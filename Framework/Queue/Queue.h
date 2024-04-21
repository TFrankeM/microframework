#ifndef QUEUE_H
#define QUEUE_H

#include <vector>
#include <mutex>
#include <limits>  // For INT_MAX
#include <condition_variable>
#include "../Dataframe/Dataframe.h"

/**
 * @brief A thread-safe queue class for managing a queue of DataFrames.
 */
class Queue {
private:
    std::vector<DataFrame> queue;
    mutable std::mutex mutex;
    std::condition_variable not_empty;  // Used to wait for a condition where the queue is not empty
    std::condition_variable not_full;   // Used to wait for a condition where the queue is not full
    const size_t max_size;              // Maximum size of the queue

public:
    // Constructor with an optional max size
    explicit Queue(size_t max_size = std::numeric_limits<size_t>::max());

    // Inserts a DataFrame into the queue
    void enqueue(const DataFrame& dataframe);   // Producer operation

    // Removes and returns a DataFrame from the queue
    DataFrame dequeue();                        // Consumer operation

    // Checks if the queue is empty
    bool isEmpty() const;

    // Checks if the queue is full
    bool isFull() const;

    // Returns the number of DataFrames in the queue
    int size() const;
};

#endif // QUEUE_H
