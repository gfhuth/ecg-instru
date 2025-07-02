const char MAIN_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 ECG</title>
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body>
  <h2>ECG Real-Time Plot</h2>
  <p><strong>BPM:</strong> <span id="bpm">0</span></p>
  <p><strong>Hora atual:</strong> <span id="currentTime">00:00:00</span></p>
  <button onclick="startStopRecording()" id="recordBtn">Iniciar Registro</button>
  <button onclick="downloadCSV()" id="downloadBtn" disabled>Baixar CSV</button>
  <button onclick="syncNTP()">Sincronizar NTP</button>
  <button onclick="syncBrowserTime()">Sincronizar com Navegador</button>
  <canvas id="ecgChart" width="800" height="400"></canvas>

  <script>
    const MAX_SAMPLES = 1000;
    let dataBuffer = new Array(MAX_SAMPLES).fill(0);
    let recording = false;

    const ctx = document.getElementById('ecgChart').getContext('2d');
    const chart = new Chart(ctx, {
      type: 'line',
      data: {
        labels: Array.from({length: MAX_SAMPLES}, (_, i) => (i / 200).toFixed(2)),
        datasets: [{
          label: 'ECG',
          data: dataBuffer,
          borderColor: 'red',
          tension: 0.1,
          pointRadius: 0
        }]
      },
      options: {
        animation: false,
        responsive: true,
        scales: {
          x: { title: { display: true, text: 'Tempo (s)' } },
          y: { title: { display: true, text: 'ADC' }, suggestedMin: 0, suggestedMax: 4095 }
        }
      }
    });

    async function fetchData() {
      const response = await fetch('/ecg');
      const values = await response.json();
      dataBuffer.push(...values);
      dataBuffer = dataBuffer.slice(-MAX_SAMPLES);
      chart.data.labels = Array.from({length: dataBuffer.length}, (_, i) => (i / 200).toFixed(2));
      chart.data.datasets[0].data = dataBuffer;
      chart.update();
    }

    async function updateBPM() {
      const r = await fetch('/bpm');
      const j = await r.json();
      document.getElementById('bpm').innerText = j.bpm;
    }

    async function updateTime() {
      const r = await fetch('/time');
      const j = await r.text();
      document.getElementById('currentTime').innerText = j;
    }

    setInterval(fetchData, 200);
    setInterval(updateBPM, 1000);
    setInterval(updateTime, 1000);

    function startStopRecording() {
      recording = !recording;
      document.getElementById('recordBtn').innerText = recording ? 'Parar Registro' : 'Iniciar Registro';
      document.getElementById('downloadBtn').disabled = recording;
      fetch('/start?mode=' + (recording ? 'start' : 'stop'));
    }

    function downloadCSV() {
      window.location.href = '/download';
    }

    function syncNTP() {
      fetch('/syncNTP');
    }

    function syncBrowserTime() {
      const epoch = Math.floor(Date.now() / 1000);
      fetch('/syncBrowserTime', {
        method: 'POST',
        headers: {'Content-Type': 'application/json'},
        body: JSON.stringify({epoch: epoch})
      });
    }
  </script>
</body>
</html>
)rawliteral";