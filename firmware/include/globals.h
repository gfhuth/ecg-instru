#ifndef GLOBALS_H
#define GLOBALS_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <esp_adc_cal.h>

// ============================================================================
// DECLARAÇÕES DE VARIÁVEIS GLOBAIS
// ============================================================================

// Handles das tasks
extern TaskHandle_t producer_task_handle;
extern TaskHandle_t consumer_task_handle;

// Semaforo para sincronização entre produtor e consumidor
extern SemaphoreHandle_t buffer_semaphore;

// Calibração do ADC
extern esp_adc_cal_characteristics_t adc_chars;

// ============================================================================
// DECLARAÇÕES DE FUNÇÕES
// ============================================================================

// Função de inicialização do buffer circular
void init_circular_buffer();

#endif // GLOBALS_H 