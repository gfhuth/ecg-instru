#include <Arduino.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "config.h"
#include "main.h"

// ============================================================================
// VARIÁVEIS GLOBAIS
// ============================================================================

circular_buffer_t circular_buffer;

// Spin‑lock para proteger o buffer
static portMUX_TYPE buffer_mux = portMUX_INITIALIZER_UNLOCKED;


// ============================================================================
// FUNÇÕES AUXILIARES PROTEGIDAS
// ============================================================================

bool is_buffer_empty()
{
    bool empty;
    portENTER_CRITICAL(&buffer_mux);
    empty = (circular_buffer.inicio == circular_buffer.fim) && !circular_buffer.overflow;
    portEXIT_CRITICAL(&buffer_mux);
    return empty;
}

bool is_buffer_full()
{
    bool full;
    portENTER_CRITICAL(&buffer_mux);
    full = (circular_buffer.inicio == circular_buffer.fim) && circular_buffer.overflow;
    portEXIT_CRITICAL(&buffer_mux);
    return full;
}

bool add_sample_to_buffer(uint32_t timestamp, uint16_t adc_value)
{
    bool ok = true;

    portENTER_CRITICAL(&buffer_mux);

    if ((circular_buffer.fim + 1) % BUFFER_SIZE == circular_buffer.inicio) {
        // Buffer está prestes a ficar cheio, marca overflow e descarta a amostra
        circular_buffer.overflow = true;
        ok = false;
    } else {
        circular_buffer.buffer[circular_buffer.fim] = {
            timestamp,
            adc_value,
            circular_buffer.fim
        };

        circular_buffer.fim = (circular_buffer.fim + 1) % BUFFER_SIZE;
    }

    portEXIT_CRITICAL(&buffer_mux);
    return ok;
}

bool get_sample_from_buffer(sample_t* sample) {
    portENTER_CRITICAL(&buffer_mux);
    
    if (is_buffer_empty()) {
        portEXIT_CRITICAL(&buffer_mux);
        return false;
    }
    
    *sample = circular_buffer.buffer[circular_buffer.inicio];
    circular_buffer.inicio = (circular_buffer.inicio + 1) % BUFFER_SIZE;
    
    if (circular_buffer.overflow && circular_buffer.inicio == circular_buffer.fim) {
        circular_buffer.overflow = false;
    }
    
    portEXIT_CRITICAL(&buffer_mux);
    return true;
}

// ============================================================================
// TASK DO PRODUTOR
// ============================================================================

void producer_task(void *parameter)
{
    const TickType_t xDelay = pdMS_TO_TICKS(1000 / SAMPLING_RATE_HZ);
    
    while (true) {
        uint32_t timestamp = millis();
        uint16_t adc_reading = adc1_get_raw(ADC_UNIT);

        if (add_sample_to_buffer(timestamp, adc_reading)) {
        } else {
            Serial.println("ERRO: Buffer cheio - perda de dados!");
        }
        vTaskDelay(xDelay);
    }
}

// ============================================================================
// TASK DO CONSUMIDOR
// ============================================================================


#define TAU 250  // ajuste conforme desejado

uint32_t filtro(uint32_t yn, uint32_t xn_p1, uint32_t dt) {
    dt = dt < 1 ? 1 : dt;
    
    float a = ((float)TAU) / (TAU + dt);
    float b = ((float)dt) / (TAU + dt);

    uint32_t yn_scaled = (uint32_t)yn * a;
    uint32_t xn_scaled = (uint32_t)xn_p1 * b;

    uint32_t yn_p1 = (yn_scaled + xn_scaled);

    return yn_p1;
}

void consumer_task(void *parameter)
{
    const TickType_t xDelay = pdMS_TO_TICKS(CONSUMER_TRIGGER_MS);
    uint32_t sig_filtrado = 2000;
    uint32_t ultimo_timestamp = 0;
    uint32_t dt = 1;
    uint32_t timestamp = millis();

    while (true) {
        sample_t sample_local;
        // uint32_t tempo_inicio, tempo_fim, tempo_consumo;
        while (get_sample_from_buffer(&sample_local)) {

            dt = sample_local.timestamp_ms - ultimo_timestamp;
            if (dt > 25){
                ultimo_timestamp = sample_local.timestamp_ms;
                sig_filtrado = filtro(sig_filtrado, sample_local.adc_value, dt);
            }
            Serial.printf("%lu,%u,%u,%u,%u,%lu\n",
                          millis(),
                          sample_local.adc_value,
                          sample_local.buffer_index,
                          circular_buffer.inicio,
                          circular_buffer.fim,
                          sig_filtrado);
        }
        vTaskDelay(xDelay);
    }
}

void loop() {
    // Pode ficar vazio, pois tudo é feito nas tasks
}
