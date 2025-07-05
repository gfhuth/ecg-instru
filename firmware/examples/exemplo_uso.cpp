/*
 * Exemplo de Uso - Sistema de Aquisição Analógica ESP32
 * 
 * Este exemplo demonstra como configurar e usar o sistema
 * para diferentes aplicações.
 */

#include <Arduino.h>
#include "config.h"

// ============================================================================
// EXEMPLO 1: MONITORAMENTO DE TEMPERATURA
// ============================================================================

void exemplo_temperatura() {
    Serial.println("=== EXEMPLO: MONITORAMENTO DE TEMPERATURA ===");
    
    // Configurações específicas para sensor de temperatura
    // Sensor LM35: 10mV/°C, 0-100°C = 0-1000mV
    
    // Para usar com sensor de temperatura, modifique config.h:
    // #define ADC_ATTEN ADC_ATTEN_DB_11  // 0-3.3V para maior faixa
    // #define SAMPLING_RATE_HZ 10        // Temperatura muda lentamente
    
    Serial.println("Configurações recomendadas:");
    Serial.println("- ADC_ATTEN: ADC_ATTEN_DB_11 (0-3.3V)");
    Serial.println("- SAMPLING_RATE_HZ: 10 Hz");
    Serial.println("- CONSUMER_TRIGGER_MS: 100 ms");
    Serial.println("- FATOR_BUFFER: 2");
    Serial.println();
}

// ============================================================================
// EXEMPLO 2: AQUISIÇÃO DE ÁUDIO
// ============================================================================

void exemplo_audio() {
    Serial.println("=== EXEMPLO: AQUISIÇÃO DE ÁUDIO ===");
    
    // Para áudio, precisamos de alta frequência de amostragem
    // Frequência de Nyquist: 2x a maior frequência do sinal
    
    Serial.println("Configurações recomendadas para áudio:");
    Serial.println("- SAMPLING_RATE_HZ: 8000 Hz (voz) ou 44100 Hz (música)");
    Serial.println("- CONSUMER_TRIGGER_MS: 10 ms");
    Serial.println("- FATOR_BUFFER: 5");
    Serial.println("- ADC_ATTEN: ADC_ATTEN_DB_11 (0-3.3V)");
    Serial.println();
}

// ============================================================================
// EXEMPLO 3: MONITORAMENTO DE VIBRAÇÃO
// ============================================================================

void exemplo_vibracao() {
    Serial.println("=== EXEMPLO: MONITORAMENTO DE VIBRAÇÃO ===");
    
    // Sensores de vibração podem ter frequências de 1-1000 Hz
    
    Serial.println("Configurações recomendadas para vibração:");
    Serial.println("- SAMPLING_RATE_HZ: 2000 Hz");
    Serial.println("- CONSUMER_TRIGGER_MS: 20 ms");
    Serial.println("- FATOR_BUFFER: 4");
    Serial.println("- ADC_ATTEN: ADC_ATTEN_DB_11 (0-3.3V)");
    Serial.println();
}

// ============================================================================
// EXEMPLO 4: MONITORAMENTO DE CORRENTE
// ============================================================================

void exemplo_corrente() {
    Serial.println("=== EXEMPLO: MONITORAMENTO DE CORRENTE ===");
    
    // Sensores de corrente como ACS712
    // Saída: 2.5V ± 0.185V/A
    
    Serial.println("Configurações recomendadas para corrente:");
    Serial.println("- SAMPLING_RATE_HZ: 100 Hz");
    Serial.println("- CONSUMER_TRIGGER_MS: 50 ms");
    Serial.println("- FATOR_BUFFER: 3");
    Serial.println("- ADC_ATTEN: ADC_ATTEN_DB_11 (0-3.3V)");
    Serial.println();
}

// ============================================================================
// EXEMPLO 5: SISTEMA DE ALARME
// ============================================================================

void exemplo_alarme() {
    Serial.println("=== EXEMPLO: SISTEMA DE ALARME ===");
    
    // Sistema que detecta mudanças bruscas no sinal
    
    Serial.println("Configurações recomendadas para alarme:");
    Serial.println("- SAMPLING_RATE_HZ: 500 Hz");
    Serial.println("- CONSUMER_TRIGGER_MS: 10 ms");
    Serial.println("- FATOR_BUFFER: 3");
    Serial.println("- ADC_ATTEN: ADC_ATTEN_DB_11 (0-3.3V)");
    Serial.println();
}

// ============================================================================
// FUNÇÃO PRINCIPAL
// ============================================================================

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n=== EXEMPLOS DE USO - SISTEMA DE AQUISIÇÃO ANALÓGICA ===");
    Serial.println();
    
    // Executa todos os exemplos
    exemplo_temperatura();
    exemplo_audio();
    exemplo_vibracao();
    exemplo_corrente();
    exemplo_alarme();
    
    Serial.println("=== INSTRUÇÕES DE USO ===");
    Serial.println("1. Copie o arquivo main.cpp para seu projeto");
    Serial.println("2. Copie o arquivo config.h para a pasta include/");
    Serial.println("3. Modifique config.h conforme o exemplo desejado");
    Serial.println("4. Compile e faça upload para o ESP32");
    Serial.println("5. Monitore a saída serial");
    Serial.println();
    
    Serial.println("=== DICAS IMPORTANTES ===");
    Serial.println("- Sempre teste com frequências baixas primeiro");
    Serial.println("- Monitore o uso de memória");
    Serial.println("- Verifique se não há overflow no buffer");
    Serial.println("- Use o debug para otimizar as configurações");
    Serial.println();
}

void loop() {
    // Este exemplo apenas mostra as configurações
    // O sistema real está no arquivo main.cpp
    delay(10000);
    Serial.println("Sistema de exemplo ativo. Verifique os arquivos principais.");
} 