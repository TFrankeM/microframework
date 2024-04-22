#include "Handler.h"

DataFrame ExampleHandler::process(const DataFrame& input) {
    // Implementação do método process
    std::map<std::string, int> eventCounts;
    for (int i = 0; i < input.rows(); ++i) {
        std::string eventType = input[i]["Event Type"];
        eventCounts[eventType]++;
    }

    // Gerar DataFrame de saída com os resultados
    DataFrame resultDataFrame;
    resultDataFrame.addColumn("Event Type");
    resultDataFrame.addColumn("Count");
    for (const auto& pair : eventCounts) {
        resultDataFrame.addRow({pair.first, std::to_string(pair.second)});
    }

    return resultDataFrame;
}

void ExampleHandler::runThread() {
    while (true) {
        // Bloqueia o acesso à fila de entrada enquanto processa os dados
        std::lock_guard<std::mutex> lock(inputQueueMutex);

        // Verifica se há dados na fila de entrada
        if (!inputQueues.empty()) {
            // Obtém o próximo DataFrame da fila de entrada
            DataFrame input = inputQueues.front().dequeue();

            // Processa o DataFrame
            DataFrame output = process(input);

            // Bloqueia o acesso à fila de saída enquanto adiciona o resultado
            std::lock_guard<std::mutex> lock(outputQueueMutex);

            // Coloca o DataFrame de saída na fila de saída
            outputQueue.enqueue(output);
        }
    }
}
