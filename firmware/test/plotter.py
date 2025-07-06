import sys
import serial
import numpy as np
import pyqtgraph as pg
from pyqtgraph.Qt import QtWidgets
import threading
import queue

# Configuração da serial
ser = serial.Serial('/dev/tty.usbserial-0001', 115200)  # Altere conforme necessário
ser.flushInput()

# Parâmetros
window_size = 2000  # Número de amostras visíveis
max_y = 4100
num_channels = 6 # Largura fixa do eixo X (ajuste conforme necessário)

# Inicializa buffers de dados com NumPy
data_buffers = np.zeros((num_channels, window_size))

# Criação da aplicação Qt
app = QtWidgets.QApplication(sys.argv)
win = pg.GraphicsLayoutWidget(title="Plotter Serial com Eixo X = Canal 1")
win.resize(800, 600)
win.show()

# Subplots
plot1 = win.addPlot(title="Canal 1 (1º valor)")
plot1.enableAutoRange(axis='y')
curve1 = plot1.plot(pen='r')

win.nextRow()

plot2 = win.addPlot(title="Canal 2 (2º valor) e Canal 6 (filtrado)")
plot2.setYRange(0, max_y)
curve2 = plot2.plot(pen='g')
curve6 = plot2.plot(pen='c')

win.nextRow()

plot3 = win.addPlot(title="Canais 3, 4, 5 (sobrepostos)")
plot3.enableAutoRange(axis='y')
curve3 = plot3.plot(pen='b')
curve4 = plot3.plot(pen='y')
curve5 = plot3.plot(pen='m')

serial_queue = queue.Queue(maxsize=500)

def serial_reader():
    while True:
        try:
            line = ser.readline().decode().strip()
            if line:
                serial_queue.put(line)
                if serial_queue.full():
                    serial_queue.get()
        except Exception as e:
            print(f"[Erro leitura serial] {e}")

# Inicie a thread
threading.Thread(target=serial_reader, daemon=True).start()

# Função de atualização
def update():
    if not serial_queue.empty():
        try:
            line = serial_queue.get_nowait()
            if not line:
                return
            parts = list(map(int, line.split(',')))
            if len(parts) != 6:
                print(f"[ERRO] Formato inválido: {line}")
                return

            # Desloca buffers e adiciona novo ponto
            data_buffers[:, :-1] = data_buffers[:, 1:]
            data_buffers[:, -1] = parts
            x_axis = data_buffers[0]  # Usa Canal 1 como eixo X

            # Define a janela fixa do eixo X
            x_max = x_axis[-1]  # Último valor do Canal 1
            x_min = x_max - window_size  # Mantém largura fixa

            # Atualiza os gráficos
            curve1.setData(x_axis, data_buffers[0])  # Canal 1
            curve2.setData(x_axis, data_buffers[1])  # Canal 2
            curve6.setData(x_axis, data_buffers[5])  # Canal 6
            curve3.setData(x_axis, data_buffers[2])  # Canal 3
            curve4.setData(x_axis, data_buffers[3])  # Canal 4
            curve5.setData(x_axis, data_buffers[4])  # Canal 5

            # Ajusta a escala X para largura fixa
            plot1.setXRange(x_min, x_max, padding=0)
            plot2.setXRange(x_min, x_max, padding=0)
            plot3.setXRange(x_min, x_max, padding=0)

        except (ValueError, queue.Empty) as e:
            print(f"[Erro processamento] {e}")

# Timer de atualização
timer = pg.QtCore.QTimer()
timer.timeout.connect(update)
timer.start(5)

# Execução
if __name__ == '__main__':
    try:
        QtWidgets.QApplication.instance().exec_()
    finally:
        ser.close()