#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// CONFIGURAÇÕES DO SISTEMA DE AQUISIÇÃO ANALÓGICA
// ============================================================================

// ============================================================================
// CONFIGURAÇÕES DE AMOSTRAGEM
// ============================================================================

/**
 * Frequência de amostragem em Hz
 * Valor recomendado: 100-1000 Hz
 * Valores muito altos podem causar perda de dados
 */
#define SAMPLING_RATE_HZ        200

/**
 * Intervalo de trigger do consumidor em milissegundos
 * Valor recomendado: 20-100 ms
 * Valores muito baixos podem sobrecarregar a saída serial
 */
#define CONSUMER_TRIGGER_MS     40

// ============================================================================
// CONFIGURAÇÕES DO ADC
// ============================================================================

/**
 * Pino de entrada analógica
 * ESP32: GPIO 32-39 (ADC1), GPIO 0, 2, 4, 12-15, 25-27 (ADC2)
 * Nota: ADC2 não funciona quando WiFi está ativo
 */
#define ADC_PIN                35

/**
 * Canal ADC correspondente ao pino
 * ADC1_CHANNEL_0 = GPIO 36
 * ADC1_CHANNEL_1 = GPIO 37
 * ADC1_CHANNEL_2 = GPIO 38
 * ADC1_CHANNEL_3 = GPIO 39
 * ADC1_CHANNEL_4 = GPIO 32
 * ADC1_CHANNEL_5 = GPIO 33
 * ADC1_CHANNEL_6 = GPIO 34
 * ADC1_CHANNEL_7 = GPIO 35
 */
#define ADC_UNIT               ADC1_CHANNEL_7

/**
 * Atenuação do ADC
 * ADC_ATTEN_DB_0   = 0-1.1V (sem atenuação)
 * ADC_ATTEN_DB_2_5 = 0-1.5V
 * ADC_ATTEN_DB_6   = 0-2.2V
 * ADC_ATTEN_DB_11  = 0-3.3V
 */
#define ADC_ATTEN              ADC_ATTEN_DB_11

// ============================================================================
// CONFIGURAÇÕES DO BUFFER
// ============================================================================

/**
 * Fator de multiplicação para o tamanho do buffer
 * Buffer = FATOR_BUFFER * amostras/s * tempo_trigger_ms / 1000
 * Valor recomendado: 2-5
 * Valores muito baixos podem causar perda de dados
 * Valores muito altos consomem mais memória
 */
#define FATOR_BUFFER           3

/**
 * Cálculo automático do tamanho do buffer
 * Não altere esta linha - ela é calculada automaticamente
 */
#define BUFFER_SIZE  (FATOR_BUFFER * SAMPLING_RATE_HZ * CONSUMER_TRIGGER_MS / 1000)

// ============================================================================
// CONFIGURAÇÕES DE PRIORIDADE DAS TASKS
// ============================================================================

/**
 * Prioridade da task do produtor (aquisição de dados)
 * Valores: 1-24 (maior número = maior prioridade)
 * Recomendado: 3-5
 */
#define PRODUCER_TASK_PRIORITY     3

/**
 * Prioridade da task do consumidor (processamento)
 * Valores: 1-24 (maior número = maior prioridade)
 * Recomendado: 2-4
 */
#define CONSUMER_TASK_PRIORITY     2

/**
 * Prioridade do timer do consumidor
 * Valores: 1-24 (maior número = maior prioridade)
 * Recomendado: 1-2
 */
#define CONSUMER_TIMER_PRIORITY    1

// ============================================================================
// CONFIGURAÇÕES DE DEBUG
// ============================================================================

/**
 * Habilita mensagens de debug detalhadas
 * 0 = Desabilitado (melhor performance)
 * 1 = Habilitado (mais informações)
 */
#define DEBUG_ENABLED          1

/**
 * Intervalo para debug periódico em milissegundos
 * Apenas usado se DEBUG_ENABLED = 1
 */
#define DEBUG_INTERVAL_MS      5000

/**
 * Frequência de debug do produtor (a cada N amostras)
 * Apenas usado se DEBUG_ENABLED = 1
 */
#define PRODUCER_DEBUG_INTERVAL 100

// ============================================================================
// CONFIGURAÇÕES DE COMUNICAÇÃO SERIAL
// ============================================================================

/**
 * Velocidade da comunicação serial em bauds
 */
#define SERIAL_BAUD_RATE       115200

// ============================================================================
// VALIDAÇÕES DE CONFIGURAÇÃO
// ============================================================================

#if SAMPLING_RATE_HZ < 1 || SAMPLING_RATE_HZ > 10000
#error "SAMPLING_RATE_HZ deve estar entre 1 e 10000 Hz"
#endif

#if CONSUMER_TRIGGER_MS < 1 || CONSUMER_TRIGGER_MS > 1000
#error "CONSUMER_TRIGGER_MS deve estar entre 1 e 1000 ms"
#endif

#if BUFFER_SIZE < 10
#error "BUFFER_SIZE muito pequeno - pode causar perda de dados"
#endif

#if BUFFER_SIZE > 10000
#error "BUFFER_SIZE muito grande - pode consumir muita memória"
#endif

#if PRODUCER_TASK_PRIORITY < 1 || PRODUCER_TASK_PRIORITY > 24
#error "PRODUCER_TASK_PRIORITY deve estar entre 1 e 24"
#endif

#if CONSUMER_TASK_PRIORITY < 1 || CONSUMER_TASK_PRIORITY > 24
#error "CONSUMER_TASK_PRIORITY deve estar entre 1 e 24"
#endif

#endif // CONFIG_H 