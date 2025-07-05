# Guia de Instalação e Uso

## 📋 Pré-requisitos

### 1. Instalar PlatformIO

O PlatformIO é necessário para compilar e fazer upload do código para o ESP32.

#### Opção A: Via VS Code (Recomendado)
1. Instale o VS Code: https://code.visualstudio.com/
2. Abra o VS Code
3. Vá em Extensions (Ctrl+Shift+X)
4. Procure por "PlatformIO IDE"
5. Instale a extensão
6. Reinicie o VS Code

#### Opção B: Via Terminal
```bash
# Instalar via pip
pip install platformio

# Ou via pip3
pip3 install platformio
```

#### Opção C: Via Homebrew (macOS)
```bash
brew install platformio
```

### 2. Conectar o ESP32

1. Conecte o ESP32 ao computador via cabo USB
2. Identifique a porta serial:
   - **Windows**: `COM3`, `COM4`, etc.
   - **macOS/Linux**: `/dev/ttyUSB0`, `/dev/ttyACM0`, etc.

## 🔧 Compilação e Upload

### Via VS Code (Recomendado)

1. Abra o projeto no VS Code
2. Aguarde o PlatformIO carregar as dependências
3. Clique no ícone do PlatformIO na barra lateral
4. Clique em "Build" para compilar
5. Clique em "Upload" para fazer upload
6. Clique em "Monitor" para ver a saída serial

### Via Terminal

```bash
# Navegar para o diretório do projeto
cd /caminho/para/firmware

# Compilar o projeto
platformio run

# Fazer upload
platformio run --target upload

# Monitorar saída serial
platformio device monitor
```

## ⚙️ Configuração do Projeto

### 1. Arquivo de Configuração

Edite o arquivo `include/config.h` para personalizar o sistema:

```cpp
// Frequência de amostragem
#define SAMPLING_RATE_HZ        200

// Intervalo de trigger do consumidor
#define CONSUMER_TRIGGER_MS     40

// Pino analógico
#define ADC_PIN                35

// Fator de multiplicação do buffer
#define FATOR_BUFFER           3
```

### 2. Configurações Recomendadas por Aplicação

#### Monitoramento de Temperatura
```cpp
#define SAMPLING_RATE_HZ        10
#define CONSUMER_TRIGGER_MS     100
#define ADC_ATTEN              ADC_ATTEN_DB_11
#define FATOR_BUFFER           2
```

#### Aquisição de Áudio
```cpp
#define SAMPLING_RATE_HZ        8000
#define CONSUMER_TRIGGER_MS     10
#define ADC_ATTEN              ADC_ATTEN_DB_11
#define FATOR_BUFFER           5
```

#### Monitoramento de Vibração
```cpp
#define SAMPLING_RATE_HZ        2000
#define CONSUMER_TRIGGER_MS     20
#define ADC_ATTEN              ADC_ATTEN_DB_11
#define FATOR_BUFFER           4
```

## 🔌 Conexões do Hardware

### Pino Analógico (GPIO 35)
- Conecte o sinal analógico ao pino GPIO 35
- Use um divisor de tensão se necessário
- Faixa de tensão: 0-1.1V (sem atenuação)

### Alimentação
- VCC: 3.3V
- GND: Terra comum

### Exemplo de Conexão com Sensor LM35
```
LM35 VCC  → ESP32 3.3V
LM35 GND  → ESP32 GND
LM35 OUT  → ESP32 GPIO 35
```

## 📊 Monitoramento e Debug

### 1. Saída Serial

Após o upload, abra o monitor serial para ver os dados:

```
=== SISTEMA DE AQUISIÇÃO ANALÓGICA ESP32 ===
Frequência de amostragem: 200 Hz
Trigger do consumidor: 40 ms
Tamanho do buffer: 24 amostras
Pino ADC: 35
Sistema inicializado com sucesso!

=== CONSUMO DE DADOS (Timestamp: 1234 ms) ===
Elementos para processar: 8
Amostra[0]: ADC=2048, Tensão=550mV, Timestamp=1230 ms
Amostra[1]: ADC=2050, Tensão=551mV, Timestamp=1235 ms
...
```

### 2. Debug Avançado

Para habilitar debug detalhado, edite `config.h`:

```cpp
#define DEBUG_ENABLED          1
#define DEBUG_INTERVAL_MS      5000
#define PRODUCER_DEBUG_INTERVAL 100
```

## 🐛 Troubleshooting

### Problemas Comuns

#### 1. Erro de Upload
```
Error: Please specify upload_port for environment or use global --upload-port option
```
**Solução**: Especifique a porta no `platformio.ini`:
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
upload_port = /dev/ttyUSB0  # Ajuste para sua porta
```

#### 2. Buffer Overflow
```
ERRO: Buffer cheio - perda de dados!
```
**Solução**: Aumente o `FATOR_BUFFER` ou reduza a `SAMPLING_RATE_HZ`

#### 3. Sem Dados na Serial
**Solução**: 
- Verifique as conexões do hardware
- Confirme se o pino está correto
- Verifique se o sensor está funcionando

#### 4. Compilação Falha
**Solução**:
- Verifique se o PlatformIO está atualizado
- Limpe o cache: `platformio run --target clean`
- Reinstale as dependências

## 📈 Otimização de Performance

### 1. Ajuste de Prioridades
```cpp
#define PRODUCER_TASK_PRIORITY     3
#define CONSUMER_TASK_PRIORITY     2
#define CONSUMER_TIMER_PRIORITY    1
```

### 2. Tamanho do Buffer
- Muito pequeno: Perda de dados
- Muito grande: Uso excessivo de memória
- Recomendado: 2-5x amostras/s * tempo_trigger_ms / 1000

### 3. Frequência de Amostragem
- Baixa: Menos dados, menor precisão
- Alta: Mais dados, maior uso de recursos
- Recomendado: 2x a frequência máxima do sinal (Nyquist)

## 🔄 Atualizações

Para atualizar o projeto:

1. Faça backup das suas configurações
2. Baixe a versão mais recente
3. Copie suas configurações personalizadas
4. Teste com configurações padrão primeiro
5. Gradualmente aplique suas customizações

## 📞 Suporte

Se encontrar problemas:

1. Verifique este guia
2. Consulte a documentação do ESP32
3. Verifique os logs de debug
4. Abra uma issue no repositório

---

**Desenvolvido para Instrumentação Eletrônica - UFRGS** 