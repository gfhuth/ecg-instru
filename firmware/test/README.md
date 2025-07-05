# Script de Captura de Dados Seriais

Este script Python captura dados da porta serial do ESP32 e salva em arquivo CSV.

## 📋 Pré-requisitos

### Instalar dependências Python:
```bash
pip install pyserial
# ou
pip install -r requirements.txt
```

## 🚀 Como Usar

### 1. Listar portas disponíveis:
```bash
python capture_serial_data.py --list-ports
```

### 2. Captura básica (usa nome automático):
```bash
python capture_serial_data.py /dev/ttyUSB0
```

### 3. Captura com arquivo específico:
```bash
python capture_serial_data.py /dev/ttyUSB0 -o meus_dados.csv
```

### 4. Captura com duração limitada:
```bash
python capture_serial_data.py /dev/ttyUSB0 -d 60 -o teste_1min.csv
```

### 5. Captura com baudrate diferente:
```bash
python capture_serial_data.py /dev/ttyUSB0 -b 9600
```

## 📊 Formato de Saída

O arquivo CSV gerado contém:
- `timestamp_ms`: Timestamp do ESP32 (ms)
- `adc_value`: Valor do ADC (0-4095)
- `buffer_index`: Índice no buffer circular
- `capture_timestamp`: Timestamp da captura no PC

### Exemplo de saída:
```csv
timestamp_ms,adc_value,buffer_index,capture_timestamp
1234,2048,0,1703123456.789
1235,2050,1,1703123456.794
1236,2047,2,1703123456.799
```

## ⚙️ Opções Disponíveis

- `port`: Porta serial (obrigatório)
- `-b, --baudrate`: Taxa de transmissão (padrão: 115200)
- `-o, --output`: Nome do arquivo de saída
- `-d, --duration`: Duração da captura em segundos
- `--list-ports`: Lista portas seriais disponíveis

## 🛑 Como Parar

- Pressione `Ctrl+C` para parar a captura
- O script salvará automaticamente os dados coletados

## 📈 Estatísticas

O script mostra:
- Número de amostras capturadas
- Taxa de amostragem média
- Tempo total de captura
- Nome do arquivo salvo

## 🔧 Portas por Sistema Operacional

- **Windows**: `COM3`, `COM4`, etc.
- **macOS**: `/dev/tty.usbserial-*`, `/dev/ttyUSB0`
- **Linux**: `/dev/ttyUSB0`, `/dev/ttyACM0`

## 🐛 Troubleshooting

### Erro de conexão:
- Verifique se a porta está correta
- Confirme se o ESP32 está conectado
- Use `--list-ports` para ver portas disponíveis

### Dados não aparecem:
- Verifique se o ESP32 está enviando dados
- Confirme o baudrate (padrão: 115200)
- Aguarde o cabeçalho ser detectado

### Arquivo não é criado:
- Verifique permissões de escrita
- Confirme se há espaço em disco
- Verifique se o caminho existe 