#include "Trigger.h"

// Constructor for the abstract Trigger class.
Trigger::Trigger(vector<Repository *> repositories, vector<vector<Queue *>> inputQueues)
    : repositories(repositories), inputQueues(inputQueues)
{
}

// Destructor for the Trigger class. Ensures the trigger thread is properly joined.
Trigger::~Trigger()
{
    if (triggerThread.joinable())
        triggerThread.join();
}

void Trigger::insertIntoQueues(Repository *repository, vector<Queue *> queues)
{
    repository->extractData();
    vector<DataFrame> data = repository->getData();
    for (int i = 0; i < data.size(); i++)
    {
        queues[i]->enqueue(data[i]);
    }
}

void Trigger::insertData()
{
    vector<std::thread> threads;
    vector<DataFrame> data = repositories[0]->getData();
    for (int i = 0; i < inputQueues.size(); i++)
    {
        threads.push_back(std::thread(&Trigger::insertIntoQueues, this, repositories[i], inputQueues[i]));
    }

    for (auto &thread : threads)
    {
        thread.join();
    }
}

// Starts the trigger thread to run asynchronously.
void Trigger::start()
{
    triggerThread = std::thread(&Trigger::fire, this);
}

// Constructor for TimerTrigger. Initializes the interval and callback.
TimerTrigger::TimerTrigger(vector<Repository *> repositories, vector<vector<Queue *>> inputQueues, long interval)
    : Trigger(repositories, inputQueues), interval(interval)
{
}

// Implementation of the fire method for TimerTrigger. Repeatedly calls the callback at the set interval.
void TimerTrigger::fire()
{
    while (active)
    {
        insertData();
        std::this_thread::sleep_for(std::chrono::milliseconds(interval));
    }
}

// Overrides the start method to set the active flag before starting the trigger thread.
void TimerTrigger::start()
{
    active = true;
    Trigger::start();
}

// Stops the timer trigger by joining the trigger thread.
void TimerTrigger::stop()
{
    active = false;
    if (triggerThread.joinable())
        triggerThread.join();
}

RequestTrigger::RequestTrigger(vector<Repository *> repositories, vector<vector<Queue *>> inputQueues)
    : Trigger(repositories, inputQueues)
{
}

void RequestTrigger::fire()
{
    insertData();
}
