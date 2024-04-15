#ifndef DATAFRAME_H
#define DATAFRAME_H

#include <vector>
#include <string>
#include <functional>
#include "Series.h"

/**
 * @brief Class representing a DataFrame similar to structures found in data analysis libraries.
 */
class DataFrame {
public:
    // List of columns, each column is a Series of data
    std::vector<Series> columns;

    // Names of the columns
    std::vector<std::string> columnNames;

    // Add a new column to the DataFrame
    void addColumn(const Series& columnData, const std::string& columnName);
    
    // Adds a new row to the DataFrame
    void addRow(const std::vector<any>& rowData);

    // Retrieves a column by its name
    Series getColumn(const std::string& columnName);

    // Retrieves a row by its index
    std::vector<any> getRow(int index);

    // Filters columns in the DataFrame based on a predicate function
    DataFrame filter(std::function<bool(const Series&)> predicate);
};

#endif //DATAFRAME_H
