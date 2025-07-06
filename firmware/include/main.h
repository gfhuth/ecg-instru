#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
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
// DECLARAÇÕES DE VARIÁVEIS GLOBAIS
// ============================================================================

// Fila circular global
extern circular_buffer_t circular_buffer;

// Handles das tasks
extern TaskHandle_t producer_task_handle;
extern TaskHandle_t consumer_task_handle;

// Semaforo para sincronização entre produtor e consumidor
extern SemaphoreHandle_t buffer_semaphore;

// Calibração do ADC
extern esp_adc_cal_characteristics_t adc_chars;

// Contador de amostras
extern volatile uint32_t samples_produced;
extern volatile uint32_t samples_consumed;

// ============================================================================
// DECLARAÇÕES DE FUNÇÕES
// ============================================================================

// Funções auxiliares
void setup_adc();
void init_circular_buffer();
bool is_buffer_empty();
bool is_buffer_full();
uint16_t buffer_count();
bool add_sample_to_buffer(uint32_t timestamp, uint16_t adc_value);
bool get_sample_from_buffer(sample_t* sample);

// Tasks
void producer_task(void *parameter);
void consumer_task(void *parameter);

#endif // MAIN_H 