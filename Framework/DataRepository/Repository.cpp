#include "../Dataframe/Series.cpp"
#include "../Dataframe/Dataframe.cpp"
#include "Repository.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <variant>

using any = std::variant<int, float, double, std::string>;

Repository::Repository(const std::string& dirPath) : directoryPath(dirPath) {}

DataFrame Repository::load_txt() {
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

DataFrame Repository::load_httpRequest() {
    std::ifstream file(directoryPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for reading: " << directoryPath<< std::endl;
        // Returning an empty DataFrame
        return DataFrame();
    }

    DataFrame dataframe;
    // Simulation of HTTP requests only have these columns
    dataframe.columnNames = {"post", "host", "date", "content_type", "content_lenght", "body"};

    // Initialize dataframe with a series to put the rows
    for (size_t i = 0; i < dataframe.columnNames.size(); ++i) {
        Series columnData;
        dataframe.columns.push_back(columnData);
    }

    std::vector<any> values(dataframe.columnNames.size());

    int i = 0;

    std::string line;
    while (std::getline(file, line)) {
        if (i < dataframe.columnNames.size()) {
            if (line.find("POST") != std::string::npos) {
                values[i] = line;
                i = 1;
            } else if (line.find(":") != std::string::npos && i != dataframe.columnNames.size()-1) {
                values[i] = line.substr(line.find(": ") + 2);
                i++;
            } else if (i == dataframe.columnNames.size()-1 && !line.empty()) {
                values[i] = line;

                dataframe.addRow(values);

                i = 0;
            }
        }
    }

    file.close();

    return dataframe;
}

std::vector<DataFrame> Repository::load_csv() {
    std::ifstream file(directoryPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for reading: " << directoryPath<< std::endl;
        // Returning an empty vector of DataFrames
        return std::vector<DataFrame>();
    }

    std::vector<DataFrame> list;
    DataFrame dataframe;
    std::string line;
    int i = 0; // State variable to track where we are in the CSV parsing process

    while (std::getline(file, line)) {
        // If the row contains only one word, create a new DataFrame
        if (line.find(',') == std::string::npos) {
            if (i != 0) {
                list.push_back(dataframe);
            }
            dataframe = DataFrame(); // Recriar o DataFrame (ou usar um método reset() se implementado)
            i = 1;
        } else if (i==1){
            // i = 1 means it's a row of column names
            std::vector<std::string> columns;
            size_t start, end;
            size_t pos = 0;

            // Takes the first value for the columnNames list
            start = 0;
            end = line.find(",");
            std::string firstColumnName = line.substr(start, end);

            columns.push_back(firstColumnName);

            // Takes the other values for the columnNames list
            while (pos != std::string::npos) {
                start = line.find(",", pos);
                end = line.find(",", start + 1);

                std::string columnName = line.substr(start + 1, end - start - 1);

                if (start != std::string::npos) {
                    columns.push_back(columnName);
                }
                pos = end; // Move pos to the end of the value
            }

            // Set the columns to the DataFrame
            dataframe.columnNames = columns;
            for (size_t i = 0; i < dataframe.columnNames.size(); ++i) {
                Series columnData;
                dataframe.columns.push_back(columnData);
            }

            i = 2;
        } else {
            // Add new row for every row that's not a column name or a word
            std::vector<any> newRow;
            size_t start, end, v1, v2;
            size_t pos = 0;

            start = 0;
            end = line.find(",");
            v1 = line.find("\"", start + 1);
            v2 = line.find("\"", v1 + 1);

            if (v1 < end && v2 > end) {
                end = v2;
            }

            std::string firstColumnValue = line.substr(start, end);

            newRow.push_back(firstColumnValue);

            while (pos != std::string::npos) {
                start = line.find(",", pos);
                end = line.find(",", start + 1);
                v1 = line.find("\"", start + 1);
                v2 = line.find("\"", v1 + 1);

                if (v1 < end && v2 > end) {
                    end = v2;
                }
                std::string columnValue = line.substr(start + 1, end - start - 1);

                if (start != std::string::npos) {
                    newRow.push_back(columnValue);
                }
                pos = end;
            }

            dataframe.addRow(newRow);
        }
    }

    // Push back the last dataframe
    if (!dataframe.columnNames.empty()) {
        list.push_back(dataframe);
    }

    file.close();

    return list;
}

void printDataFrame(DataFrame& dataframe) {
    // Print columns
    std::cout << "Columns: ";
    for (const auto& name : dataframe.columnNames) {
        std::cout << name << "\t";
    }
    std::cout << std::endl;

    // Print dataframe
    for (int i = 0; i < dataframe.columns[0].size(); ++i) {
        for (const auto& column : dataframe.columns) {
            const any& dataframe = column.get(i);
            std::visit([&](const auto& value) {
                std::cout << value << "\t";
            }, dataframe);
        }
        std::cout << std::endl;
    }
}

// // For test
// int main() {
//     // Creating a Repository with "datacat_logs.txt" file
//     Repository repository1("datacat_logs.txt");

//     // Loading datacat_logs.txt file in a DataFrame
//     DataFrame data1 = repository1.load_txt();

//     // Creating a Repository with "cadeanalytics_events.txt" file
//     Repository repository2("cadeanalytics_events.txt");

//     // Loading cadeanalytics_events.txt file in a DataFrame
//     DataFrame data2 = repository2.load_httpRequest();

//     // Print DataFrames
//     printDataFrame(data1);
//     printDataFrame(data2);

//     // Creating a Repository with "contaverde_spreadsheets.csv" file
//     Repository repository3("contaverde_spreadsheets.csv");

//     // Loading contaverde_spreadsheets.csv file in a DataFrame
//     std::vector<DataFrame> data3 = repository3.load_csv();

//     // Print each DataFrame
//     for (auto& dataframe : data3) {
//         std::cout << "DataFrame:\n";
//         printDataFrame(dataframe);
//         std::cout << "\n";
//     }

//     return 0;
// }