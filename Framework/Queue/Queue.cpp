#include "Queue.h"
#include <stdexcept>

// Constructor for the Queue class: it initializes the max_size with
// the value of the max_size parameter passed to the constructor
Queue::Queue(size_t max_size) : max_size(max_size) {}

/**
 * @brief Inserts a DataFrame into the queue.
 * 
 * @param dataframe The DataFrame to insert.
 */
void Queue::enqueue(const DataFrame& dataframe) {
    std::unique_lock<std::mutex> lock(mutex);
    // Block if the queue is full (producer waits)
    not_full.wait(lock, [this] {
        return queue.size() < max_size; 
        });
    queue.push_back(dataframe);
    lock.unlock();
    // Notify one waiting thread (consumer) that the queue is not empty
    not_empty.notify_one();
}

/**
 * @brief Removes and returns a DataFrame from the queue.
 * 
 * @return DataFrame The next DataFrame in the queue.
 */
DataFrame Queue::dequeue() {
    std::unique_lock<std::mutex> lock(mutex);
    // Block if the queue is empty (consumer waits)
    not_empty.wait(lock, [this] {
        return !queue.empty();
    });
    DataFrame df = queue.front();
    queue.erase(queue.begin());
    lock.unlock();
    // Notify one waiting thread (producer) that the queue is not full
    not_full.notify_one();
    return df;
}

/**
 * @brief Checks if the queue is empty.
 * 
 * @return true If the queue is empty.
 * @return false If the queue has at least one DataFrame.
 */
bool Queue::isEmpty() const {
    std::lock_guard<std::mutex> lock(mutex);
    return queue.empty();
}

/**
 * @brief Checks if the queue is full.
 * 
 * @return true If the queue is full.
 * @return false If the queue has at least one free space.
 */
bool Queue::isFull() const {
    std::lock_guard<std::mutex> lock(mutex);
    return queue.size() >= max_size;
}

/**
 * @brief Returns the number of DataFrames in the queue.
 * 
 * @return int The number of DataFrames.
 */
int Queue::size() const {
    std::lock_guard<std::mutex> lock(mutex);
    return static_cast<int>(queue.size());
}
