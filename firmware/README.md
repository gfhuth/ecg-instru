# Sistema de Aquisição Analógica ESP32

Sistema completo de aquisição de dados analógicos em tempo real para ESP32, utilizando DMA e fila circular.

## 🚀 Características

- **Aquisição Contínua**: 200 Hz (configurável)
- **Fila Circular**: Buffer FIFO thread-safe
- **Arquitetura Assíncrona**: Produtor e consumidor independentes
- **Configurável**: Frequência, buffer e timing ajustáveis
- **Debug Completo**: Logging e monitoramento

## 📋 Especificações

### Hardware
- **Microcontrolador**: ESP32
- **Pino Analógico**: GPIO 35 (ADC1_CHANNEL_7)
- **Faixa**: 0-1.1V (sem atenuação)
- **Resolução**: 12 bits

### Software
- **Framework**: Arduino (PlatformIO)
- **Sistema**: FreeRTOS
- **Frequência**: 200 Hz
- **Trigger**: 40 ms
- **Buffer**: 24 amostras (calculado automaticamente)

## ⚙️ Configuração

Edite `include/config.h` para personalizar:

```cpp
#define SAMPLING_RATE_HZ        200    // Frequência de amostragem
#define CONSUMER_TRIGGER_MS     40     // Intervalo do consumidor
#define ADC_PIN                35      // Pino analógico
#define FATOR_BUFFER           3       // Multiplicador do buffer
```

## 🔧 Compilação

```bash
# Compilar e upload
pio run --target upload

# Monitorar saída
pio device monitor
```

## 📊 Saída

```
=== CONSUMO DE DADOS (Timestamp: 1234 ms) ===
Amostra[0]: ADC=2048, Tensão=550mV, Timestamp=1230 ms
Amostra[1]: ADC=2050, Tensão=551mV, Timestamp=1235 ms
...
```

## 🏗️ Arquitetura

- **Produtor**: Core 1, prioridade 3, aquisição contínua
- **Consumidor**: Core 0, prioridade 2, processamento
- **Timer**: Trigger assíncrono a cada 40ms
- **Fila Circular**: Thread-safe com controle de overflow

## 📝 Aplicações

- Monitoramento de sensores
- Aquisição de sinais analógicos
- Análise de dados em tempo real
- Sistemas de instrumentação

---

**Desenvolvido para Instrumentação Eletrônica - UFRGS** 