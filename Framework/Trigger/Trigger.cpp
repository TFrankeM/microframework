#include "Trigger.h"

// Constructor for the abstract Trigger class.
Trigger::Trigger(Queue* queue) 
    : outputQueue(queue) 
    {}

// Destructor for the Trigger class. Ensures the trigger thread is properly joined.
Trigger::~Trigger() {
    if (triggerThread.joinable()) 
        triggerThread.join();
}

// Starts the trigger thread to run asynchronously.
void Trigger::start() {
    triggerThread = std::thread(&Trigger::fire, this);
}

// Constructor for TimerTrigger. Initializes the interval and callback.
TimerTrigger::TimerTrigger(Queue* queue, long interval, std::function<void()> callback)
    : Trigger(queue)
    , interval(interval)
    , callback(callback) 
    {}

// Implementation of the fire method for TimerTrigger. Repeatedly calls the callback at the set interval.
void TimerTrigger::fire() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(interval));
        callback();
    }
}

RequestTrigger::RequestTrigger(Queue* queue) 
    : Trigger(queue) 
    {}

void RequestTrigger::fire() {
}

void RequestTrigger::waitForRequest() {
    fire();
}
