#include "Trigger.h"

// Constructor for the abstract Trigger class.
Trigger::Trigger(Queue *queue, std::function<void()> callback)
    : outputQueue(queue), callback(callback)
{
}

// Destructor for the Trigger class. Ensures the trigger thread is properly joined.
Trigger::~Trigger()
{
    if (triggerThread.joinable())
        triggerThread.join();
}

// Starts the trigger thread to run asynchronously.
void Trigger::start()
{
    triggerThread = std::thread(&Trigger::fire, this);
}

// Constructor for TimerTrigger. Initializes the interval and callback.
TimerTrigger::TimerTrigger(Queue *queue, std::function<void()> callback, long interval)
    : Trigger(queue, callback), interval(interval)
{
}

// Implementation of the fire method for TimerTrigger. Repeatedly calls the callback at the set interval.
void TimerTrigger::fire()
{
    while (active)
    {
        callback();
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

RequestTrigger::RequestTrigger(Queue *queue, std::function<void()> callback)
    : Trigger(queue, callback)
{
}

void RequestTrigger::fire()
{
    callback();
}
