#include <Arduino.h>
#include "setup.h"
#include "web_server.h"

void loop() {
    // Executa a tarefa do consumidor a cada consumerRateMs milissegundos
    static uint32_t lastConsumerTime = 0;
    uint32_t currentTime = millis();
    
    if (currentTime - lastConsumerTime >= consumerRateMs) {
        consumerTask();
        lastConsumerTime = currentTime;
    }
    
    // Processa requisições do servidor web
    handleWebServer();
    
    // Pequeno delay para não sobrecarregar o sistema
    delay(1);
}