#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include <driver/i2s.h>
#include <driver/adc.h>
#include "esp_task_wdt.h"
#include "my_wifi.h"  

// ADC CONFIG
i2s_port_t i2s_port = I2S_NUM_0;
adc1_channel_t adc_channel = ADC1_CHANNEL_7; // GPIO35
const uint16_t adc_sample_freq = 800;      // mudou pra 1000

const uint16_t dma_buffer_len = 1024;
const uint16_t i2s_buffer_len = dma_buffer_len;
const uint16_t ws_tx_buffer_len = dma_buffer_len;

uint16_t* i2s_read_buff = (uint16_t*)calloc(i2s_buffer_len, sizeof(uint16_t));
char ws_send_buffer[100];  // buffer para texto de envio via WS

AsyncWebServer server(80);
AsyncWebSocket ws("/test");
AsyncWebSocketClient * globalClient = NULL;

boolean streaming = true;
boolean sampling  = true;
size_t bytes_read;
TaskHandle_t samplingTaskHandle;


volatile uint16_t latestRawSample = 0;    // último dado do buffer (atualizado em getSamples())
volatile uint16_t filteredValue = 2048;   // valor filtrado (mesmo tipo do sinal original)
volatile uint32_t lastFilterTime = 0;

const uint32_t tau = 100 * 1000;        // tau em micros 
const uint32_t T_filter = 1000;   // intervalo do filtro em micros


uint16_t y_prev = 2048;        // valor inicial do filtro (meio ADC 12bit)
uint32_t last_filter_time = 0;

void configure_i2s(){
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
    .sample_rate = adc_sample_freq,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = 0,
    .dma_buf_count = 4,
    .dma_buf_len = dma_buffer_len,
    .use_apll = 0,
  };
  adc1_config_channel_atten(adc_channel, ADC_ATTEN_11db);
  adc1_config_width(ADC_WIDTH_12Bit);
  i2s_driver_install(i2s_port, &i2s_config, 0, NULL);
 
  i2s_set_adc_mode(ADC_UNIT_1, adc_channel);
  i2s_adc_enable(i2s_port);
  vTaskDelay(1000);
}

void serverons(){
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    while(!SPIFFS.exists("/index.html")){
      Serial.println("Error, /index.html is not onboard");
      delay(1000);
    }
    Serial.println("trying to send index.html");
    AsyncWebServerResponse* response = request->beginResponse(SPIFFS, "/index.html", "text/html");
    response->addHeader("Access-Control-Max-Age", "10000");
    response->addHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
    response->addHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
    request->send(response);
    Serial.println("sent");
  });

  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Sending /favicon.ico");
    request->send(SPIFFS, "/favicon.ico","text/css");
  });

  server.on("/jqueryjs", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("sending /jquery.js.gz");
    AsyncWebServerResponse* response = request->beginResponse(SPIFFS, "/jquery.js.gz", "text/javascript");
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  server.on("/CanvasJs", HTTP_GET, [](AsyncWebServerRequest* request) {
    Serial.println("Sending /CanvasJs.js.gz");
    AsyncWebServerResponse* response = request->beginResponse(SPIFFS, "/CanvasJs.js.gz", "text/javascript");
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });
  
  server.on("/Stop", HTTP_GET, [](AsyncWebServerRequest * request) {
    Serial.println("----STOP----");
    streaming = false;
    sampling = false;
    request->send(204);
  });
  
  server.on("/Start", HTTP_GET, [](AsyncWebServerRequest * request) {
    Serial.println("----START----");
    streaming = true;
    sampling = true;
    request->send(204);
  });
}

static const inline void getSamples();
static const inline void sendSamples();

static void getDataLoopTask(void * pvParameters){
  for( ;; ){
    if(sampling){
      getSamples();
      delayMicroseconds(100);
      esp_task_wdt_reset();
    } else {
      vTaskDelay(1);
    }
  }
}

static const inline void getSamples(){
  i2s_read(i2s_port, (void*)i2s_read_buff, i2s_buffer_len*sizeof(uint16_t), &bytes_read, portMAX_DELAY);
  if(streaming && bytes_read>0){
    latestRawSample = i2s_read_buff[(bytes_read/2) - 1] & 0x0FFF;
    sendSamples();
  }
}

// filtro discreto, usando diferença de tempo em micros
// filtro discreto, usando diferença de tempo em micros
void filterTask(void* pvParameters) {
  unsigned long currentTime = micros();
  lastFilterTime = currentTime;

  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(T_filter / 1000)); // delay em ms

    unsigned long now = micros();
    unsigned long T = now - lastFilterTime;
    lastFilterTime = now;

    // filtro exponencial: y[n] = (tau * y[n-1] + T * x[n]) / (tau + T)
    uint16_t y_prev = filteredValue;
    uint16_t x = latestRawSample;

    // Calcula com precisão float e converte para uint16_t
    float result = (tau * y_prev + T * x) / (float)(tau + T);
    filteredValue = (uint16_t)result;
  }
}

static const inline void sendSamples(){
  static uint16_t ws_send_buffer_filtered[ws_tx_buffer_len * 2];

  for(int i=0; i<ws_tx_buffer_len; i++){
    uint16_t rawSample = i2s_read_buff[i] & 0x0FFF;
    uint16_t filteredSample = filteredValue;  // mesmo tipo, sem cast necessário

    ws_send_buffer_filtered[2*i] = rawSample;
    ws_send_buffer_filtered[2*i + 1] = filteredSample;
  }
  ws.binaryAll((uint8_t*)ws_send_buffer_filtered, ws_tx_buffer_len * 2 * sizeof(uint16_t));
}


void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type,
  void * arg, uint8_t *data, size_t len){

  switch(type){
    case WS_EVT_CONNECT:
      Serial.println("WebSocket client connected");
      globalClient = client;
      break;
    case WS_EVT_DISCONNECT:
      Serial.println("WebSocket client disconnected");
      globalClient = NULL;
      break;
    default:
      break;
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  if(!SPIFFS.begin(true)){
    Serial.println("SPIFFS mount failed");
    return;
  }

  configure_i2s();

  serverons();
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.begin();

  xTaskCreatePinnedToCore(filterTask, "FilterTask", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(getDataLoopTask, "SampleLoop", 2048, NULL, 1, &samplingTaskHandle, 0);
}

void loop() {
  esp_task_wdt_reset();
  delay(1000);
}
