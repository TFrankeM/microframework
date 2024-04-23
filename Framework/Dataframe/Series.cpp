#include "Series.h"
#include <stdexcept>

/**
 * @brief Adds a new element to the series.
 * 
 * @param value The element to add.
 */
void Series::add(const any& value) {
    data.push_back(value);
}

/**
 * @brief Inserts a new element at a specific index.
 * 
 * @param index The index at which to insert the element.
 * @param value The element to insert.
 */
void Series::insert(int index, const any& value) {
    if (index < 0 || index > data.size()) {
        throw std::out_of_range("Index out of range");
    }
    data.insert(data.begin() + index, value);
}

/**
 * @brief Retrieves an element at a specific index.
 * 
 * @param index The index of the element to retrieve.
 * @return any The element at the specified index.
 */
any Series::get(int index) const {
    if (index < 0 || index >= data.size()) {
        throw std::out_of_range("Index out of range");
    }
    return data[index];
}

// ... Other Series methods ...

/**
 * @brief Gets the size of the series.
 * 
 * @return size_t The number of elements in the series.
 */
size_t Series::size() const {
    return data.size();
}
