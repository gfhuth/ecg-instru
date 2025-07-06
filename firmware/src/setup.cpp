#include "main.h"
#include <driver/adc.h>
#include <esp_adc_cal.h>

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
        1                        // Core
    );
    
    Serial.println("Sistema inicializado com sucesso!");
    Serial.println("Aguardando dados...\n");
}

// ============================================================================
// FUNÇÃO DE CONFIGURAÇÃO DO ADC
// ============================================================================

void setup_adc() {
    // Configura o ADC
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC_UNIT, ADC_ATTEN_11db);
    
    // Caracteriza o ADC (ADC_UNIT_1 é a unidade, ADC_UNIT é o canal)
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_11db, ADC_WIDTH_BIT_12, 
                            1100, &adc_chars);
    
    Serial.println("ADC configurado com sucesso!");
} 