#ifndef HANDLER_H
#define HANDLER_H

#include <vector>
#include <mutex>
#include "../Queue/Queue.h"
#include "../Dataframe/Dataframe.h"

class Handler {
public:
    // Método virtual puro para processar um DataFrame de entrada e produzir um ou mais DataFrames como saída
    virtual DataFrame process(const DataFrame& input) = 0;

    // Método virtual para executar o tratamento de dados
    virtual void runThread() = 0;

    // Recebe as filas de entrada e saída
    Handler(std::vector<Queue<DataFrame>>& inputQueues, Queue<DataFrame>& outputQueue)
        : inputQueues(inputQueues), outputQueue(outputQueue) {}

protected:
    std::vector<Queue<DataFrame>>& inputQueues; // Filas de entrada
    Queue<DataFrame>& outputQueue; // Fila de saída

    // Mutexes para controlar o acesso às filas de entrada e saída
    std::mutex inputQueueMutex; 
    std::mutex outputQueueMutex;
};

class ExampleHandler : public Handler {
public:
    ExampleHandler(std::vector<Queue<DataFrame>>& inputQueues, Queue<DataFrame>& outputQueue)
        : Handler(inputQueues, outputQueue) {}

    // Implementação do método process
    DataFrame process(const DataFrame& input) override;

    // Implementação do método runThread
    void runThread() override;
};

#endif // HANDLER_H

