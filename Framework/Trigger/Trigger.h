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
class Trigger
{
protected:
    // Queue to hold DataFrames for processing
    Queue *outputQueue;
    // Thread to handle the asynchronous triggering
    std::thread triggerThread;
    // Callback function to be called on trigger
    std::function<void()> callback;

public:
    explicit Trigger(Queue *queue, std::function<void()> callback);
    virtual ~Trigger();
    virtual void fire() = 0; // Pure virtual function to be implemented by derived classes
    virtual void start();    // Starts the trigger thread
};

/**
 * @brief The TimerTrigger class invokes a callback function at regular intervals.
 *
 * It's useful for periodic tasks like polling or scheduled data processing.
 */
class TimerTrigger : public Trigger
{
private:
    // Interval in milliseconds between each trigger
    long interval;
    // Flag to indicate if the timer trigger is active
    bool active = false;

public:
    TimerTrigger(Queue *queue, std::function<void()> callback, long interval);
    // Implementation of the fire method for timer-based triggering
    void fire() override;
    void start() override; // Overrides the start method to set the active flag
    void stop();           // Stops the timer trigger
};

/**
 * @brief The RequestTrigger class is designed to be triggered by external events or requests.
 *
 * For example, it could be used to trigger processing when a network request is received.
 */
class RequestTrigger : public Trigger
{
public:
    RequestTrigger(Queue *queue, std::function<void()> callback);
    // Implementation of the fire method for request-based triggering
    void fire() override;
};

#endif // TRIGGER_H
