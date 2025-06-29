#include <WiFi.h>
#include <WebServer.h>
#include <Ticker.h>

Ticker sampler;

const char* ssid = "POCO F3";
const char* password = "fernando";

WebServer server(80);

const int ecgPin = 35;
const int sampleRateHz = 200;
const int bufferSize = 1000;  // 5 seconds of data at 1000 Hz

uint16_t ecgBuffer[bufferSize];
int bufferIndex = 0;

// Timer for sampling
hw_timer_t* timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  ecgBuffer[bufferIndex] = analogRead(ecgPin);
  bufferIndex = (bufferIndex + 1) % bufferSize;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 ECG Graph</title>
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body>
  <h2>ECG Real-Time Plot</h2>
  <canvas id="ecgChart" width="800" height="400"></canvas>
  <script>
    const ctx = document.getElementById('ecgChart').getContext('2d');
    const MAX_SAMPLES = 1000; // 5 seconds at 10000 Hz
    let dataBuffer = new Array(MAX_SAMPLES).fill(0);

    const chart = new Chart(ctx, {
      type: 'line',
      data: {
        labels: Array.from({length: MAX_SAMPLES}, (_, i) => (i / 200).toFixed(2)),
        datasets: [{
          label: 'ECG',
          data: dataBuffer,
          borderColor: 'red',
          tension: 0.1,
          pointRadius: 0
        }]
      },
      options: {
        animation: false,
        responsive: true,
        scales: {
          x: {
            title: { display: true, text: 'Time (s)' }
          },
          y: {
            title: { display: true, text: 'ADC Value' },
            suggestedMin: 0,
            suggestedMax: 4095
          }
        }
      }
    });

    async function fetchData() {
      try {
        const response = await fetch('/ecg');
        const newValues = await response.json();

        // Append values, trim to last 10000 samples
        dataBuffer.push(...newValues);
        dataBuffer = dataBuffer.slice(-MAX_SAMPLES);

        chart.data.labels = Array.from({length: dataBuffer.length}, (_, i) => (i / 200).toFixed(2));
        chart.data.datasets[0].data = dataBuffer;
        chart.update();
      } catch (e) {
        console.error("Error fetching ECG data:", e);
      }
    }

    setInterval(fetchData, 200); // every 10000 ms
  </script>
</body>
</html>
)rawliteral";
  server.send(200, "text/html", html);
}

void handleECG() {
  const int sampleChunk = 40;
  String json = "[";
  portENTER_CRITICAL(&timerMux);
  int i = (bufferIndex - sampleChunk + bufferSize) % bufferSize;
  for (int count = 0; count < sampleChunk; count++) {
    json += String(ecgBuffer[i]);
    if (count < sampleChunk - 1) json += ",";
    i = (i + 1) % bufferSize;
  }
  portEXIT_CRITICAL(&timerMux);
  json += "]";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! IP: ");
  Serial.println(WiFi.localIP());

  analogReadResolution(12); // 0–4095

  server.on("/", handleRoot);
  server.on("/ecg", handleECG);
  server.begin();

  sampler.attach_ms(5, []() {
    portENTER_CRITICAL_ISR(&timerMux);
    ecgBuffer[bufferIndex] = analogRead(ecgPin);
    bufferIndex = (bufferIndex + 1) % bufferSize;
    portEXIT_CRITICAL_ISR(&timerMux);
  });
}

void loop() {
  server.handleClient();
}
