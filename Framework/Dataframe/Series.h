#ifndef SERIES_H
#define SERIES_H

#include <vector>
#include <string>
#include <variant>

// Defines 'any' as a type capable of holding multiple primitive types.
using any = std::variant<int, float, double, std::string>; // Add other types as needed

/**
 * @brief Class representing a series of data. A series is similar to a column in a DataFrame.
 */
class Series {
private:
    // Internal storage for the series data
    std::vector<any> data;

public:
    // Adds a new element to the series
    void add(const any& value);

    // Inserts a new element at a specific index
    void insert(int index, const any& value);

    // Retrieves an element at a specific index
    any get(int index) const;

    // Method to get the size of the series
    size_t size() const;
};

#endif //SERIES_H
