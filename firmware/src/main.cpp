#include <Arduino.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/timers.h>
#include "config.h"

// ============================================================================
// ESTRUTURAS DE DADOS
// ============================================================================

// Estrutura para armazenar uma amostra com timestamp
typedef struct {
    uint32_t timestamp_ms;    // Timestamp em milissegundos
    uint16_t adc_value;       // Valor do ADC
    uint16_t buffer_index;    // Índice no buffer
} sample_t;

// Estrutura da fila circular
typedef struct {
    sample_t buffer[BUFFER_SIZE];  // Buffer de amostras
    volatile uint16_t inicio;      // Índice de início (próxima posição a ler)
    volatile uint16_t fim;         // Índice de fim (próxima posição a escrever)
    volatile bool overflow;        // Flag de overflow
} circular_buffer_t;

// ============================================================================
// VARIÁVEIS GLOBAIS
// ============================================================================

// Fila circular global
circular_buffer_t circular_buffer;

// Handles das tasks e timers
TaskHandle_t producer_task_handle = NULL;
TaskHandle_t consumer_task_handle = NULL;
TimerHandle_t consumer_timer_handle = NULL;

// Semaforo para sincronização entre produtor e consumidor
SemaphoreHandle_t buffer_semaphore;

// Calibração do ADC
esp_adc_cal_characteristics_t adc_chars;

// Contador de amostras para debug
volatile uint32_t samples_produced = 0;
volatile uint32_t samples_consumed = 0;

// ============================================================================
// FUNÇÕES AUXILIARES
// ============================================================================

/**
 * Inicializa o ADC com as configurações especificadas
 */
void setup_adc() {
    // Configura o ADC
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC_UNIT, ADC_ATTEN);
    
    // Caracteriza o ADC
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN, ADC_WIDTH_BIT_12, 
                            1100, &adc_chars);
    
    Serial.println("ADC inicializado com sucesso");
}

/**
 * Inicializa a fila circular
 */
void init_circular_buffer() {
    circular_buffer.inicio = 0;
    circular_buffer.fim = 0;
    circular_buffer.overflow = false;
    
    // Inicializa o buffer com zeros
    memset(circular_buffer.buffer, 0, sizeof(circular_buffer.buffer));
    
    Serial.printf("Fila circular inicializada com tamanho: %d\n", BUFFER_SIZE);
}

/**
 * Verifica se a fila circular está vazia
 */
bool is_buffer_empty() {
    return circular_buffer.inicio == circular_buffer.fim && !circular_buffer.overflow;
}

/**
 * Verifica se a fila circular está cheia
 */
bool is_buffer_full() {
    return circular_buffer.inicio == circular_buffer.fim && circular_buffer.overflow;
}

/**
 * Calcula o número de elementos na fila circular
 */
uint16_t buffer_count() {
    if (circular_buffer.overflow) {
        return BUFFER_SIZE;
    }
    
    if (circular_buffer.fim >= circular_buffer.inicio) {
        return circular_buffer.fim - circular_buffer.inicio;
    } else {
        return BUFFER_SIZE - circular_buffer.inicio + circular_buffer.fim;
    }
}

/**
 * Adiciona uma amostra na fila circular (produtor)
 */
bool add_sample_to_buffer(uint32_t timestamp, uint16_t adc_value) {
    // Verifica se o buffer está cheio
    if (is_buffer_full()) {
        circular_buffer.overflow = true;
        return false;
    }
    
    // Adiciona a amostra
    circular_buffer.buffer[circular_buffer.fim].timestamp_ms = timestamp;
    circular_buffer.buffer[circular_buffer.fim].adc_value = adc_value;
    circular_buffer.buffer[circular_buffer.fim].buffer_index = circular_buffer.fim;
    
    // Atualiza o índice de fim
    circular_buffer.fim = (circular_buffer.fim + 1) % BUFFER_SIZE;
    
    // Se o fim alcançou o início, marca como overflow
    if (circular_buffer.fim == circular_buffer.inicio) {
        circular_buffer.overflow = true;
    }
    
    return true;
}

// ============================================================================
// TASK DO PRODUTOR (AQUISIÇÃO DE DADOS)
// ============================================================================

void producer_task(void *parameter) {
    const TickType_t xDelay = pdMS_TO_TICKS(1000 / SAMPLING_RATE_HZ); // Delay em ticks
    uint32_t sample_count = 0;
    
    Serial.println("Task do produtor iniciada");
    
    while (true) {
        // Lê o valor do ADC
        uint32_t adc_reading = adc1_get_raw(ADC_UNIT);
        
        // Converte para tensão (opcional, para debug)
        uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, &adc_chars);
        
        // Obtém o timestamp atual
        uint32_t timestamp = millis();
        
        // Adiciona a amostra ao buffer
        if (add_sample_to_buffer(timestamp, adc_reading)) {
            samples_produced++;
            sample_count++;
            
            // Debug a cada N amostras (configurável)
            #if DEBUG_ENABLED
            if (sample_count % PRODUCER_DEBUG_INTERVAL == 0) {
                Serial.printf("Produtor: %d amostras produzidas, Buffer: %d/%d\n", 
                            sample_count, buffer_count(), BUFFER_SIZE);
            }
            #endif
        } else {
            Serial.println("ERRO: Buffer cheio - perda de dados!");
        }
        
        // Aguarda o próximo ciclo de amostragem
        vTaskDelay(xDelay);
    }
}

// ============================================================================
// TASK DO CONSUMIDOR (LEITURA E PROCESSAMENTO)
// ============================================================================

void consumer_task(void *parameter) {
    // Cabeçalho já impresso no setup
    while (true) {
        // Aguarda o sinal do timer
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        
        // Conta quantos elementos há para processar
        uint16_t elements_to_process = buffer_count();
        
        if (elements_to_process > 0) {
            // Processa todos os elementos disponíveis
            uint16_t processed = 0;
            while (!is_buffer_empty() && processed < elements_to_process) {
                // Lê a amostra do buffer
                sample_t sample = circular_buffer.buffer[circular_buffer.inicio];
                
                // Imprime apenas os dados numéricos separados por vírgula
                Serial.printf("%lu,%u,%u\n", sample.timestamp_ms, sample.adc_value, sample.buffer_index);
                
                // Atualiza o índice de início
                circular_buffer.inicio = (circular_buffer.inicio + 1) % BUFFER_SIZE;
                
                // Se havia overflow, limpa a flag
                if (circular_buffer.overflow && circular_buffer.inicio == circular_buffer.fim) {
                    circular_buffer.overflow = false;
                }
                
                processed++;
                samples_consumed++;
            }
        }
    }
}

// ============================================================================
// CALLBACK DO TIMER DO CONSUMIDOR
// ============================================================================

void consumer_timer_callback(TimerHandle_t xTimer) {
    // Notifica a task do consumidor
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(consumer_task_handle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// ============================================================================
// FUNÇÃO PRINCIPAL DE SETUP
// ============================================================================

void setup() {
    // Inicializa a comunicação serial
    Serial.begin(SERIAL_BAUD_RATE);
    // Cabeçalho para CSV
    Serial.println("timestamp_ms,adc_value,buffer_index");
    Serial.println("\n=== SISTEMA DE AQUISIÇÃO ANALÓGICA ESP32 ===");
    Serial.printf("Frequência de amostragem: %d Hz\n", SAMPLING_RATE_HZ);
    Serial.printf("Trigger do consumidor: %d ms\n", CONSUMER_TRIGGER_MS);
    Serial.printf("Tamanho do buffer: %d amostras\n", BUFFER_SIZE);
    Serial.printf("Pino ADC: %d\n", ADC_PIN);
    
    // Inicializa o ADC
    setup_adc();
    
    // Inicializa a fila circular
    init_circular_buffer();
    
    // Cria o semáforo para sincronização
    buffer_semaphore = xSemaphoreCreateBinary();
    
    // Cria a task do produtor
    xTaskCreatePinnedToCore(
        producer_task,           // Função da task
        "Producer",              // Nome da task
        4096,                    // Tamanho da stack
        NULL,                    // Parâmetros
        PRODUCER_TASK_PRIORITY,  // Prioridade
        &producer_task_handle,   // Handle da task
        1                        // Core (1 para evitar conflitos com WiFi)
    );
    
    // Cria a task do consumidor
    xTaskCreatePinnedToCore(
        consumer_task,           // Função da task
        "Consumer",              // Nome da task
        4096,                    // Tamanho da stack
        NULL,                    // Parâmetros
        CONSUMER_TASK_PRIORITY,  // Prioridade
        &consumer_task_handle,   // Handle da task
        0                        // Core
    );
    
    // Cria o timer para trigger do consumidor
    consumer_timer_handle = xTimerCreate(
        "ConsumerTimer",         // Nome do timer
        pdMS_TO_TICKS(CONSUMER_TRIGGER_MS),  // Período
        pdTRUE,                  // Auto-reload
        NULL,                    // ID do timer
        consumer_timer_callback  // Callback
    );
    
    // Inicia o timer
    xTimerStart(consumer_timer_handle, 0);
    
    Serial.println("Sistema inicializado com sucesso!");
    Serial.println("Aguardando dados...\n");
}

// ============================================================================
// FUNÇÃO PRINCIPAL DE LOOP
// ============================================================================

void loop() {
    // O loop principal fica vazio pois tudo é feito nas tasks
    // Apenas um delay para não sobrecarregar o sistema
    delay(1000);
    
    // Debug periódico
    #if DEBUG_ENABLED
    static uint32_t last_debug = 0;
    if (millis() - last_debug > DEBUG_INTERVAL_MS) {
        Serial.printf("Status: Produzidas=%lu, Consumidas=%lu, Buffer=%d/%d\n",
                    samples_produced, samples_consumed, buffer_count(), BUFFER_SIZE);
        last_debug = millis();
    }
    #endif
} 