#include "Repository.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <variant>

using any = std::variant<int, float, double, std::string>;

Repository::Repository(const std::string& dirPath) : directoryPath(dirPath) {}

DataFrame Repository::loadData_txt() {
    std::ifstream file(directoryPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for reading: " << directoryPath<< std::endl;
        // Returning an empty DataFrame
        return DataFrame();
    }

    // Getting the column names
    std::vector<std::string> columns;
    std::string line1;

    while (std::getline(file, line1)) {
        size_t posf = line1.find(":");
        while (posf != std::string::npos) {
            size_t start, end, dots;

            // Search for the words between "":
            start = line1.find("\"", posf);
            end = line1.find("\"", start + 1);
            dots = line1.find(":", end);

            if (end == dots-1) {
                bool rep = false;
                std::string columnName = line1.substr(start + 1, end - start - 1);
                // Don't repeat columns
                for (int i = 0; i < columns.size(); i++){
                    if (columns[i] == columnName){
                        rep = true;
                    }
                }
                if (rep == false){
                    columns.push_back(columnName);
                }
            }

            posf = end;
        }
    }
    
    // Clear file to use again
    file.clear();
    file.seekg(0);

    // Define dataframe
    DataFrame dataframe;
    dataframe.columnNames = columns;

    // Initialize dataframe with a series to put the rows
    for (size_t i = 0; i < dataframe.columnNames.size(); ++i) {
        Series columnData;
        dataframe.columns.push_back(columnData);
    }

    std::string line;
    while (std::getline(file, line)) {
        std::vector<any> values(dataframe.columnNames.size());
        size_t pos = line.find(":");

        // Finding the columns on the .txt file
        for (size_t i = 0; i < dataframe.columnNames.size(); ++i) {
            std::string columnName = dataframe.columnNames[i];
            std::string searchString = "\"" + columnName + "\":";
            if (pos != std::string::npos) {
                size_t pos_num, end_num, startPos;

                startPos = line.find(searchString, pos);
                if (startPos != std::string::npos) {
                    // If there's a numerical value next the column name, takes its position
                    pos_num = line.find_first_of("1234567890.", startPos); 
                    end_num = line.find_first_not_of("1234567890.", pos_num);
                    startPos = line.find("\"", startPos + searchString.size());
                    size_t endPos = line.find("\"", startPos + 1);

                    if (end_num < startPos && line[end_num] == ','){
                        startPos = pos_num-1;
                        endPos = end_num;
                    }

                    // Get the initial and final positions of the value and add to the row vector
                    if (endPos != std::string::npos) {
                        values[i] = line.substr(startPos + 1, endPos - startPos - 1);
                        startPos = endPos; // Move pos to the end of the value
                    }
                } else {
                    // If that row don't have a value for that column, its null ("")
                    values[i] = "";
                }
            }
        }

        dataframe.addRow(values);
    }

    file.close();

    return dataframe;
}

// For test
// int main() {
//     // Creating a Repository with "datacat_logs.txt" file
//     Repository repository("datacat_logs.txt");

//     // Loading datacat_logs.txt file in a DataFrame
//     DataFrame data = repository.loadData_txt();

//     // Print columns
//     std::cout << "Columns: ";
//     for (const auto& name : data.columnNames) {
//         std::cout << name << "\t";
//     }
//     std::cout << std::endl;

//     // Print dataframe
//     for (int i = 0; i < data.columns[0].size(); ++i) {
//         for (const auto& column : data.columns) {
//             const any& data = column.get(i);
//             std::visit([&](const auto& value) {
//                 std::cout << value << "\t";
//             }, data);
//         }
//         std::cout << std::endl;
//     }

//     return 0;
// }