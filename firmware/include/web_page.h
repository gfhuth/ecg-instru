#ifndef WEB_PAGE_H
#define WEB_PAGE_H

const char WEB_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 ADC - Tempo Real</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 20px;
            background-color: #f0f0f0;
        }
        .container {
            max-width: 1200px;
            margin: 0 auto;
            background: white;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }
        .controls {
            margin-bottom: 20px;
            padding: 15px;
            background: #f8f9fa;
            border-radius: 5px;
        }
        .checkbox-group {
            display: flex;
            gap: 20px;
            align-items: center;
        }
        .checkbox-item {
            display: flex;
            align-items: center;
            gap: 5px;
        }
        .checkbox-item input[type="checkbox"] {
            width: 18px;
            height: 18px;
        }
        .status {
            margin-top: 10px;
            padding: 10px;
            border-radius: 5px;
            font-weight: bold;
        }
        .status.connected {
            background: #d4edda;
            color: #155724;
            border: 1px solid #c3e6cb;
        }
        .status.disconnected {
            background: #f8d7da;
            color: #721c24;
            border: 1px solid #f5c6cb;
        }
        canvas {
            border: 1px solid #ddd;
            border-radius: 5px;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>📊 ESP32 ADC - Monitoramento em Tempo Real</h1>
        
        <div class="controls">
            <h3>🎛️ Controles de Visualização</h3>
            <div class="checkbox-group">
                <div class="checkbox-item">
                    <input type="checkbox" id="showADC" checked>
                    <label for="showADC">📈 Valor ADC (Vermelho)</label>
                </div>
                <div class="checkbox-item">
                    <input type="checkbox" id="showIndex" checked>
                    <label for="showIndex">📊 Índice Buffer (Verde)</label>
                </div>
                <div class="checkbox-item">
                    <input type="checkbox" id="showTimestamp" checked>
                    <label for="showTimestamp">⏰ Timestamp (Azul)</label>
                </div>
            </div>
            <div class="status" id="status">Conectando...</div>
        </div>

        <canvas id="adcChart" width="800" height="400"></canvas>
    </div>

    <script>
        const MAX_POINTS = 100;
        let adcData = [];
        let indexData = [];
        let timestampData = [];
        let chart;

        // Inicializa o gráfico
        function initChart() {
            const ctx = document.getElementById('adcChart').getContext('2d');
            chart = new Chart(ctx, {
                type: 'line',
                data: {
                    labels: Array.from({length: MAX_POINTS}, (_, i) => i),
                    datasets: [{
                        label: 'Valor ADC',
                        data: adcData,
                        borderColor: 'red',
                        backgroundColor: 'rgba(255, 0, 0, 0.1)',
                        tension: 0.1,
                        pointRadius: 0,
                        hidden: !document.getElementById('showADC').checked
                    }, {
                        label: 'Índice Buffer',
                        data: indexData,
                        borderColor: 'green',
                        backgroundColor: 'rgba(0, 255, 0, 0.1)',
                        tension: 0.1,
                        pointRadius: 0,
                        hidden: !document.getElementById('showIndex').checked
                    }, {
                        label: 'Timestamp (ms)',
                        data: timestampData,
                        borderColor: 'blue',
                        backgroundColor: 'rgba(0, 0, 255, 0.1)',
                        tension: 0.1,
                        pointRadius: 0,
                        hidden: !document.getElementById('showTimestamp').checked
                    }]
                },
                options: {
                    responsive: true,
                    animation: false,
                    scales: {
                        x: {
                            title: {
                                display: true,
                                text: 'Amostras'
                            }
                        },
                        y: {
                            title: {
                                display: true,
                                text: 'Valor'
                            }
                        }
                    },
                    plugins: {
                        legend: {
                            display: true
                        }
                    }
                }
            });
        }

        // Atualiza os dados do gráfico
        function updateChart() {
            fetch('/data')
                .then(response => response.json())
                .then(data => {
                    if (data.samples && data.samples.length > 0) {
                        // Adiciona novos dados
                        data.samples.forEach(sample => {
                            adcData.push(sample.adcValue);
                            indexData.push(sample.bufferIndex);
                            timestampData.push(sample.timestamp % 1000); // Mostra apenas os últimos 3 dígitos
                        });

                        // Mantém apenas os últimos MAX_POINTS
                        if (adcData.length > MAX_POINTS) {
                            adcData = adcData.slice(-MAX_POINTS);
                            indexData = indexData.slice(-MAX_POINTS);
                            timestampData = timestampData.slice(-MAX_POINTS);
                        }

                        // Atualiza o gráfico
                        chart.data.datasets[0].data = adcData;
                        chart.data.datasets[1].data = indexData;
                        chart.data.datasets[2].data = timestampData;
                        chart.update('none');

                        // Atualiza status
                        document.getElementById('status').textContent = 
                            `Conectado - ${data.samples.length} amostras recebidas`;
                        document.getElementById('status').className = 'status connected';
                    }
                })
                .catch(error => {
                    console.error('Erro ao buscar dados:', error);
                    document.getElementById('status').textContent = 'Erro de conexão';
                    document.getElementById('status').className = 'status disconnected';
                });
        }

        // Event listeners para os checkboxes
        document.getElementById('showADC').addEventListener('change', function() {
            chart.data.datasets[0].hidden = !this.checked;
            chart.update();
        });

        document.getElementById('showIndex').addEventListener('change', function() {
            chart.data.datasets[1].hidden = !this.checked;
            chart.update();
        });

        document.getElementById('showTimestamp').addEventListener('change', function() {
            chart.data.datasets[2].hidden = !this.checked;
            chart.update();
        });

        // Inicializa o gráfico e inicia as atualizações
        initChart();
        setInterval(updateChart, 100); // Atualiza a cada 100ms
    </script>
</body>
</html>
)rawliteral";

#endif 