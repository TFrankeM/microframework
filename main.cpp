#include "Framework/DataRepository/Repository.h"
#include "Framework/DataRepository/Extractor.h"
#include "Framework/Dataframe/Dataframe.h"
#include "Framework/Queue/Queue.h"
#include "Framework/Handler/Handler.h"
#include <iostream>

using std::cout;
using std::endl;

const string CSV_DIRECTORY = "C:/Users/pedro/Desktop/Computacao_Escalavel/microframework/contaverde/";
const string DATACAT_DIRECTORY = "C:/Users/pedro/Desktop/Computacao_Escalavel/microframework/datacat/";

int main()
{
    Extractor *userExtractor = new FileExtractor(CSV_DIRECTORY + "users.csv", DataFormat::CSV);
    Extractor *productExtractor = new FileExtractor(CSV_DIRECTORY + "products.csv", DataFormat::CSV);
    Extractor *orderExtractor = new FileExtractor(CSV_DIRECTORY + "orders.csv", DataFormat::CSV);
    Extractor *inventoryExtractor = new FileExtractor(CSV_DIRECTORY + "inventory.csv", DataFormat::CSV);

    Repository userRepository(userExtractor);
    Repository productRepository(productExtractor);
    Repository orderRepository(orderExtractor);
    Repository inventoryRepository(inventoryExtractor);

    // Só nos interessa o comportamento do usuário
    Extractor *datacatExtractor = new DirectoryMultipleExtractor(DATACAT_DIRECTORY, DataFormat::JSON, ".txt", "type", {"user_behavior"});
    Repository dataCatRepository(datacatExtractor);

    Extractor *cadeAnalyticsExtractor = new RequestExtractor();
    Repository cadeAnalyticsRepository(cadeAnalyticsExtractor);

    Queue userBehaviourQueue;
    Queue outputQueue1;
    Queue outputQueue2;
    Handler *copyHandler = new CopyHandler({&userBehaviourQueue}, {&outputQueue1, &outputQueue2}, 2);

    dataCatRepository.extractData();
    DataFrame dataCat = dataCatRepository.getData()[0];
    cout << "DataCat:" << endl;
    dataCat.print();
    userBehaviourQueue.enqueue(dataCat);

    copyHandler->run();

    DataFrame output1 = outputQueue1.dequeue();
    DataFrame output2 = outputQueue2.dequeue();

    cout << "Output 1:" << endl;
    output1.print();

    cout << "Output 2:" << endl;
    output2.print();
}