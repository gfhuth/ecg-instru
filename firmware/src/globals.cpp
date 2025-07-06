#include "globals.h"
#include "main.h"

// ============================================================================
// DEFINIÇÕES DE VARIÁVEIS GLOBAIS
// ============================================================================

// Handles das tasks
TaskHandle_t producer_task_handle = NULL;
TaskHandle_t consumer_task_handle = NULL;

// Semaforo para sincronização entre produtor e consumidor
SemaphoreHandle_t buffer_semaphore = NULL;

// Calibração do ADC
esp_adc_cal_characteristics_t adc_chars;

// ============================================================================
// FUNÇÕES GLOBAIS
// ============================================================================

void init_circular_buffer() {
    // Inicializa a fila circular
    circular_buffer.inicio = 0;
    circular_buffer.fim = 0;
    circular_buffer.overflow = false;
    
} 