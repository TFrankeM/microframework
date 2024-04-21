#ifndef TRIGGER_H
#define TRIGGER_H

#include <thread>
#include <functional>
#include <chrono>
#include "../Queue/Queue.h"

/**
 * @brief The Trigger class serves as an abstract base for various trigger mechanisms.
 * 
 * It manages a thread that executes a triggering action asynchronously.
 */
class Trigger {
protected:
    // Queue to hold DataFrames for processing
    Queue* outputQueue;
    // Thread to handle the asynchronous triggering
    std::thread triggerThread;

public:
    explicit Trigger(Queue* queue);
    virtual ~Trigger();
    virtual void fire() = 0; // Pure virtual function to be implemented by derived classes
    void start(); // Starts the trigger thread
};

/**
 * @brief The TimerTrigger class invokes a callback function at regular intervals.
 * 
 * It's useful for periodic tasks like polling or scheduled data processing.
 */
class TimerTrigger : public Trigger {
private:
    // Interval in milliseconds between each trigger
    long interval;
    // Callback function to be called on trigger
    std::function<void()> callback;

public:
    TimerTrigger(Queue* queue, long interval, std::function<void()> callback);
    // Implementation of the fire method for timer-based triggering
    void fire() override;
};

/**
 * @brief The RequestTrigger class is designed to be triggered by external events or requests.
 * 
 * For example, it could be used to trigger processing when a network request is received.
 */
class RequestTrigger : public Trigger {
public:
    RequestTrigger(Queue* queue);
    // Implementation of the fire method for request-based triggering
    void fire() override;
    // Method to simulate or wait for an external request
    void waitForRequest();
};

#endif // TRIGGER_H
