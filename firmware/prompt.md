Gere um código para o microcontrolador ESP32 que realize a aquisição contínua de dados analógicos em tempo real utilizando DMA e uma fila circular, ao mesmo tempo um consumidor deve ler os dados e jogar na saida serial.

*   **Produtor (Entrada Analógica):** Utilize o pino 35 para a entrada analógica para preencher um buffer tipo fila circular. Utilize o ADC1, sem atenuação, se possível.
*   **Frequência de Amostragem (Sampling Rate):** A frequência de amostragem inicial deve ser de 200 amostras por segundo (Hz). Permita que a frequência de amostragem seja configurável.
*   **Buffer FIFO (Fila Circular):** Implemente uma fila circular para armazenar os dados adquiridos. Inclua as variáveis `inicio` e `fim` para controlar os índices da fila circular. O buffer + indice de inicio + indice de fim, podem estar num struct.
*   **Consumidor:** O consumidor deve ler todos os dados do buffer FIFO entre os elementos indicados pelo indice de inicio e fim da fila circular, imprimir o timestamp (em milissegundos) daquele burst de consumo, o valor da entrada analógica e seus respectivos índices do buffer na saída serial. Após a leitura do buffer, o índice `inicio` deve ser atualizado para o último índice `fim`. O consumidor é trigged de maneira assincrona e independente do Produtor. Faça alguma maneira de fazer um scheduler de 40ms.
*   **Tamanho do Buffer:** O tamanho do buffer deve ser qualquer valor e não pode estar atrelado a nada, deve ser configurado pelo usuário. Inicialmente para que seja 3x amostras/s do produtor * tempo entre trigger do consumidor.
*   **Configurabilidade:** Permita a configuração da frequência de amostragem, tamanho do buffer e taxa de execução do consumidor através de variáveis no código.
*   **Assincronismo:** Lembre de garantir que o consumidor e o produtor são independentes e assíncronos! Precisa de um mecanismo (interrupt, flag, etc.) para saber onde o produtor está e onde a consumidor pode ler.


Por favor, inclua comentários no código para explicar o funcionamento de cada seção.