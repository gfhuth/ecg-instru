# Sistema de Aquisição de Dados Analógicos - ESP32

Este projeto implementa um sistema completo de aquisição de dados analógicos usando o ESP32, com DMA (Direct Memory Access), fila circular (FIFO) e interface web em tempo real.

## 🎯 Características

- **Entrada Analógica**: Pino 35 (ADC1_CH7)
- **Frequência de Amostragem**: 200 Hz (configurável)
- **Taxa do Consumidor**: 25 ms (configurável)
- **Buffer Circular**: Tamanho calculado automaticamente
- **DMA**: Transferência automática de dados do ADC
- **Interface Web**: Gráficos em tempo real via WiFi
- **Resolução**: 12 bits (0-4095)

## 📁 Estrutura do Projeto

```
firmware/
├── include/
│   ├── setup.h          # Configurações principais
│   ├── sampling.h       # Declarações das funções de amostragem
│   ├── web_server.h     # Declarações do servidor web
│   ├── web_page.h       # Página HTML da interface
│   └── wifi_config.h    # Credenciais WiFi (não versionado)
├── src/
│   ├── main.cpp         # Loop principal
│   ├── setup.cpp        # Inicialização do sistema
│   ├── sampling.cpp     # Funções de aquisição de dados
│   └── web_server.cpp   # Servidor web e WiFi
└── platformio.ini       # Configuração do PlatformIO
```

## ⚙️ Configurações

### Frequência de Amostragem
```cpp
const uint32_t sampleRateHz = 200;  // Hz
```

### Taxa do Consumidor
```cpp
const uint32_t consumerRateMs = 25;  // ms
```

### WiFi
Edite `include/wifi_config.h`:
```cpp
const char* ssid = "SUA_REDE_WIFI";
const char* password = "SUA_SENHA_WIFI";
```

## 🌐 Interface Web

### Acesso
1. Conecte o ESP32 à rede WiFi
2. Anote o IP exibido no Serial Monitor
3. Acesse `http://[IP_DO_ESP32]` no navegador

### Funcionalidades
- **Gráfico em Tempo Real**: Dados movem da esquerda para a direita
- **Três Curvas**:
  - 🔴 **Vermelho**: Valor ADC (0-4095)
  - 🟢 **Verde**: Índice do Buffer (0-4)
  - 🔵 **Azul**: Timestamp (últimos 3 dígitos)
- **Controles**: Checkboxes para mostrar/ocultar cada curva
- **Status**: Indicador de conexão e amostras recebidas

## 🔧 Como Funciona

### 1. **Inicialização**
- Conecta ao WiFi
- Inicia servidor web na porta 80
- Configura ADC1 no pino 35
- Inicia timer de amostragem

### 2. **Amostragem**
- Timer dispara a cada 5ms (200Hz)
- ADC converte o sinal analógico
- Dados armazenados no buffer circular

### 3. **Consumidor**
- Executa a cada 25ms
- Lê dados do buffer FIFO
- Envia para interface web via JSON
- Imprime no Serial Monitor

### 4. **Interface Web**
- Atualiza a cada 100ms
- Exibe gráfico com Chart.js
- Permite controle de visualização

## 📊 Saídas

### Serial Monitor
```
=== SISTEMA DE AQUISIÇÃO DE DADOS ANALÓGICOS ===
ESP32 com DMA, Fila Circular e Interface Web
================================================
Frequência de amostragem: 200 Hz
Taxa do consumidor: 25 ms
Tamanho do buffer: 5 amostras
Pino ADC: 35 (Canal ADC1_CH7)
================================================
WiFi conectado!
Endereço IP: 192.168.1.100
Servidor web iniciado
Sistema inicializado com sucesso!
Acesse http://192.168.1.100 para ver os dados em tempo real

Timestamp(ms) | Valor ADC | Índice Buffer
----------------------------------------
        12345 |      2048 |            0
        12346 |      2050 |            1
        12347 |      2045 |            2
```

### Interface Web
- Gráfico interativo com 3 curvas coloridas
- Controles para mostrar/ocultar dados
- Status de conexão em tempo real

## 🚀 Como Usar

1. **Configure o WiFi** em `include/wifi_config.h`
2. **Conecte o sinal analógico** ao pino 35
3. **Compile e faça upload** do código
4. **Abra o Serial Monitor** (115200 baud)
5. **Anote o IP** exibido
6. **Acesse a interface web** no navegador

## ⚠️ Considerações

- **Segurança**: Credenciais WiFi não são versionadas
- **Performance**: Interface atualiza a cada 100ms
- **Buffer**: 50 amostras mantidas para web
- **Overflow**: Detectado automaticamente

## 🔄 Personalização

Para alterar as configurações, modifique as constantes em `include/setup.h`:

```cpp
// Aumentar frequência de amostragem
const uint32_t sampleRateHz = 1000;  // 1 kHz

// Alterar taxa do consumidor
const uint32_t consumerRateMs = 10;   // 10 ms

// O tamanho do buffer será recalculado automaticamente
```

## 📈 Monitoramento

O sistema fornece:
- Configurações atuais
- Status de conexão WiFi
- Dados em tempo real (Serial + Web)
- Overflow do buffer
- Performance do sistema 