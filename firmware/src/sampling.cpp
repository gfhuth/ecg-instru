#include <Arduino.h>
#include "setup.h"
#include "sampling.h"
#include "web_server.h"
#include "driver/adc.h"
#include "driver/dac.h"
#include "esp_adc_cal.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_timer.h"

// Buffer circular para armazenar os dados
volatile uint16_t adcBuffer[bufferSize];
volatile uint32_t bufferInicio = 0;
volatile uint32_t bufferFim = 0;
volatile bool bufferOverflow = false;

// Configuração do ADC
esp_adc_cal_characteristics_t adc_chars;

// Timer para controle da frequência de amostragem
esp_timer_handle_t samplingTimer;

// Fila para comunicação entre DMA e consumidor
QueueHandle_t dataQueue;

/**
 * Callback do timer de amostragem
 * Esta função é chamada periodicamente para iniciar uma nova conversão ADC
 */
void IRAM_ATTR samplingTimerCallback(void* arg) {
    // Lê o valor do ADC diretamente
    uint16_t adcValue = adc1_get_raw((adc1_channel_t)adcChannel);
    
    // Calcula o próximo índice na fila circular
    uint32_t nextIndex = (bufferFim + 1) % bufferSize;
    
    // Verifica se há espaço no buffer
    if (nextIndex != bufferInicio) {
        // Armazena o valor no buffer
        adcBuffer[bufferFim] = adcValue;
        bufferFim = nextIndex;
        
        // Adiciona ao buffer web (fora do IRAM_ATTR)
        // Nota: addWebSample será chamada no loop principal
    } else {
        // Buffer cheio - overflow
        bufferOverflow = true;
    }
}

/**
 * Configura o ADC1 para o canal especificado
 */
void setupADC() {
    Serial.println("Configurando ADC...");
    
    // Configura o ADC1
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten((adc1_channel_t)adcChannel, ADC_ATTEN_11db);
    
    // Caracterização do ADC para melhor precisão
    esp_adc_cal_characterize((adc_unit_t)adcUnit, ADC_ATTEN_11db, ADC_WIDTH_BIT_12, 1100, &adc_chars);
    
    Serial.printf("ADC configurado - Canal: %d, Pino: %d\n", adcChannel, adcPin);
}

/**
 * Configura o DMA para transferência de dados do ADC
 */
void setupDMA() {
    Serial.println("Configurando DMA...");
    
    // O ESP32 tem DMA integrado no ADC, mas estamos usando timer + leitura direta
    // para simplicidade e controle total sobre a frequência de amostragem
    
    Serial.println("Sistema de amostragem configurado");
}

/**
 * Inicia o timer de amostragem
 */
void startSampling() {
    Serial.println("Iniciando amostragem...");
    
    // Configura o timer para a frequência de amostragem especificada
    esp_timer_create_args_t timerConfig = {
        .callback = &samplingTimerCallback,
        .arg = NULL,
        .name = "sampling_timer"
    };
    
    esp_timer_create(&timerConfig, &samplingTimer);
    
    // Inicia o timer periódico
    uint64_t periodUs = 1000000 / sampleRateHz; // Converte Hz para microssegundos
    esp_timer_start_periodic(samplingTimer, periodUs);
    
    Serial.printf("Amostragem iniciada - Frequência: %d Hz, Período: %llu us\n", 
                  sampleRateHz, periodUs);
}

/**
 * Para o timer de amostragem
 */
void stopSampling() {
    Serial.println("Parando amostragem...");
    
    if (samplingTimer) {
        esp_timer_stop(samplingTimer);
        esp_timer_delete(samplingTimer);
        samplingTimer = NULL;
    }
    
    Serial.println("Amostragem parada");
}

/**
 * Tarefa do consumidor que lê os dados do buffer FIFO
 */
void consumerTask() {
    uint32_t samplesRead = 0;
    
    // Lê todos os dados disponíveis no buffer
    while (bufferInicio != bufferFim) {
        // Lê o valor do buffer
        uint16_t adcValue = adcBuffer[bufferInicio];
        
        // Obtém o timestamp atual
        uint32_t timestamp = millis();
        
        // Imprime os dados no Serial
        Serial.printf("%12lu | %9d | %12lu\n", 
                      timestamp, adcValue, bufferInicio);
        
        // Adiciona ao buffer web para envio via HTTP
        addWebSample(adcValue, bufferInicio, timestamp);
        
        // Atualiza o índice de início
        bufferInicio = (bufferInicio + 1) % bufferSize;
        samplesRead++;
    }
    
    // Só imprime informações se houve dados lidos
    if (samplesRead > 0) {
        Serial.printf("Total de amostras lidas: %lu\n", samplesRead);
    }
    
    // Verifica se houve overflow
    if (bufferOverflow) {
        Serial.println("AVISO: Buffer overflow detectado!");
        bufferOverflow = false;
    }
} 