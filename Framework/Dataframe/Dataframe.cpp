#include "Dataframe.h"
#include <stdexcept>

/**
 * @brief Adds a new column to the DataFrame.
 * 
 * @param columnData The data for the new column.
 * @param columnName The name of the new column.
 */
void DataFrame::addColumn(const Series& columnData, const std::string& columnName) {
    // Ensure that the new column has the same length as the existing ones
    if (!columns.empty() && columnData.size() != columns.front().size()) {
        throw std::invalid_argument("New column length must match existing columns.");
    }

    // Add the new column and its name to the DataFrame
    columns.push_back(columnData);
    columnNames.push_back(columnName);
}

/**
 * @brief Adds a new row to the DataFrame.
 * 
 * @param rowData The data for the new row.
 */
void DataFrame::addRow(const std::vector<any>& rowData) {
    if (rowData.size() != columns.size()) {
        throw std::invalid_argument("Row size does not match number of columns.");
    }
    
    for (size_t i = 0; i < rowData.size(); ++i) {
        columns[i].add(rowData[i]);
    }
}

/**
 * @brief Retrieves a column by its name.
 * 
 * @param columnName The name of the column to retrieve.
 * @return Series The requested column.
 */
Series DataFrame::getColumn(const std::string& columnName) {
    auto it = std::find(columnNames.begin(), columnNames.end(), columnName);
    if (it != columnNames.end()) {
        return columns[std::distance(columnNames.begin(), it)];
    }
    throw std::invalid_argument("Column name does not exist.");
}

/**
 * @brief Retrieves a row by its index.
 * 
 * @param index The index of the row to retrieve.
 * @return std::vector<any> The data in the row.
 */
std::vector<any> DataFrame::getRow(int index) {
    std::vector<any> row;
    for (auto& col : columns) {
        row.push_back(col.get(index));
    }
    return row;
}

/**
 * @brief Filters columns in the DataFrame based on a predicate function.
 * 
 * @param predicate The function used to determine which columns to include.
 * @return DataFrame A new DataFrame containing only the filtered columns.
 */
DataFrame DataFrame::filter(std::function<bool(const Series&)> predicate) {
    DataFrame newDf;
    
    for (const auto& col : columns) {
        if (predicate(col)) {
            newDf.columns.push_back(col);
            // Find the index of the column in the old DataFrame and use it to add the corresponding column name
            auto index = &col - &columns[0];
            newDf.columnNames.push_back(columnNames[index]);
        }
    }
    
    return newDf;
}
