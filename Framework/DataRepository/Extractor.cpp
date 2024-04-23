#include "Extractor.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <map>
#include <sqlite3.h>

namespace fs = std::filesystem;
using std::string;

DataFrame FileExtractor::extractFromCsv(const string &filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file for reading: " << filePath << std::endl;
        // Returning an empty DataFrame
        return DataFrame();
    }

    // Getting the column names
    std::vector<string> columns;
    string firstLine;
    std::getline(file, firstLine);

    size_t posf = firstLine.find(separator);
    size_t start = 0;
    size_t end = 0;

    while (posf != string::npos)
    {
        if (firstLine[start] == '\"')
        {
            start = firstLine.find("\"", posf);
            end = firstLine.find("\"", start + 1);
            columns.push_back(firstLine.substr(start + 1, end - start - 1));
        }
        else
        {
            end = firstLine.find(separator, posf);
            columns.push_back(firstLine.substr(posf, end - posf));
        }
        posf = end + 1;
    }

    // Define dataframe
    DataFrame dataframe;
    dataframe.columnNames = columns;

    // Getting the data
    string line2;

    while (std::getline(file, line2))
    {
        std::vector<any> row;
        size_t pos = 0;
        size_t start = 0;
        size_t end = 0;

        for (size_t i = 0; i < columns.size(); ++i)
        {
            if (line2[start] == '\"')
            {
                start = line2.find("\"", pos);
                end = line2.find("\"", start + 1);
                row.push_back(line2.substr(start + 1, end - start - 1));
            }
            else
            {
                end = line2.find(separator, pos);
                row.push_back(line2.substr(pos, end - pos));
            }
            pos = end + 1;
        }

        dataframe.addRow(row);
    }

    return dataframe;
}

DataFrame FileExtractor::extractFromJson(const string &filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file for reading: " << filePath << std::endl;
        // Returning an empty DataFrame
        return DataFrame();
    }

    // Getting the column names
    std::vector<string> columns;
    string firstLine;
    std::getline(file, firstLine);

    size_t posf = firstLine.find(":");
    while (posf != string::npos)
    {
        if (firstLine[posf - 1] == '\"')
        {
            size_t start = firstLine.find("\"", posf);
            size_t end = firstLine.find("\"", start + 1);
            columns.push_back(firstLine.substr(start + 1, end - start - 1));
        }
        posf = firstLine.find(":", posf + 1);
    }

    // Define dataframe
    DataFrame dataframe;
    dataframe.columnNames = columns;

    // Getting the data
    string line2;

    while (std::getline(file, line2))
    {
        std::vector<any> row;
        size_t pos = 0;
        size_t start = 0;
        size_t end = 0;

        for (size_t i = 0; i < columns.size(); ++i)
        {
            size_t pos_num, end_num, startPos;

            startPos = line2.find("\"", pos);
            if (startPos != std::string::npos)
            {
                pos_num = line2.find_first_of("1234567890.", startPos);
                end_num = line2.find_first_not_of("1234567890.", pos_num);
                startPos = line2.find("\"", startPos + 1);
                size_t endPos = line2.find("\"", startPos + 1);

                if (end_num < startPos && line2[end_num] == ',')
                {
                    startPos = pos_num - 1;
                    endPos = end_num;
                }

                if (endPos != std::string::npos)
                {
                    row.push_back(line2.substr(startPos + 1, endPos - startPos - 1));
                    startPos = endPos;
                }
            }
            else
            {
                row.push_back("");
            }
        }

        dataframe.addRow(row);
    }

    return dataframe;
}

DataFrame FileExtractor::extractData()
{
    if (format == DataFormat::CSV)
    {
        return extractFromCsv(path);
    }
    else if (format == DataFormat::JSON)
    {
        return extractFromJson(path);
    }
    else
    {
        std::cerr << "Invalid data format" << std::endl;
        // Returning an empty DataFrame
        return DataFrame();
    }
}

DataFrame DirectoryExtractor::extractData()
{
    DataFrame dataframe;
    for (const auto &entry : fs::directory_iterator(path))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }
        if (entry.path().extension() != extension)
        {
            continue;
        }
        
        DataFrame fileData;
        if (format == DataFormat::CSV)
        {
            fileData = extractFromCsv(entry.path().string());
        }
        else if (format == DataFormat::JSON)
        {
            fileData = extractFromJson(entry.path().string());
        }
        else
        {
            std::cerr << "Invalid data format" << std::endl;
            continue;
        }

        if (dataframe.columns.empty())
        {
            dataframe.columnNames = fileData.columnNames;
        }

        for (size_t i = 0; i < fileData.columns[0].size(); ++i)
        {
            dataframe.addRow(fileData.getRow(i));
        }
    }
    return dataframe;
}

bool RequestExtractor::handleRequest(const string &request)
{
    std::lock_guard<std::mutex> lock(dataMutex);
    // Handle the request and update the data
    data = DataFrame();

    // Check if the request is valid
    if (request != "GET /data")
    {
        return false;
    }
    
    // Extract data from the request (json)
    std::vector<string> columns;

    size_t posf = request.find(":");
    while (posf != string::npos)
    {
        std::vector<any> row;
        
        size_t start = request.find("\"", posf);
        size_t end = request.find("\"", start + 1);
        size_t dots = request.find(":", end);

        if (end == dots - 1)
        {
            bool rep = false;
            string columnName = request.substr(start + 1, end - start - 1);
            for (int i = 0; i < columns.size(); i++)
            {
                if (columns[i] == columnName)
                {
                    rep = true;
                }
            }
            if (rep == false)
            {
                columns.push_back(columnName);
            }
        }

        posf = end;
    }

    data.columnNames = columns;

    // Initialize dataframe with a series to put the rows
    for (size_t i = 0; i < data.columnNames.size(); ++i) {
        Series columnData;
        data.columns.push_back(columnData);
    }

    // Extract data from the request (json)
    size_t pos = request.find(":");
    while (pos != string::npos)
    {
        std::vector<any> row(data.columnNames.size());
        
        for (size_t i = 0; i < data.columnNames.size(); ++i) {
            std::string columnName = data.columnNames[i];
            std::string searchString = "\"" + columnName + "\":";
            if (pos != std::string::npos) {
                size_t pos_num, end_num, startPos;

                startPos = request.find(searchString, pos);
                if (startPos != std::string::npos) {
                    // If there's a numerical value next the column name, takes its position
                    pos_num = request.find_first_of("1234567890.", startPos); 
                    end_num = request.find_first_not_of("1234567890.", pos_num);
                    startPos = request.find("\"", startPos + searchString.size());
                    size_t endPos = request.find("\"", startPos + 1);

                    if (end_num < startPos && request[end_num] == ','){
                        startPos = pos_num-1;
                        endPos = end_num;
                    }

                    // Get the initial and final positions of the value and add to the row vector
                    if (endPos != std::string::npos) {
                        row[i] = request.substr(startPos + 1, endPos - startPos - 1);
                        startPos = endPos; // Move pos to the end of the value
                    }
                } else {
                    // If that row don't have a value for that column, its null ("")
                    row[i] = "";
                }
            }
        }

        data.addRow(row);
    }

    return true;
}

DataFrame RequestExtractor::extractData()
{
    std::lock_guard<std::mutex> lock(dataMutex);
    // Return the current data
    return data;
}

DatabaseExtractor::DatabaseExtractor(sqlite3 *db, const string &query) : db(db), query(query) {}

static int callback(void *data, int argc, char **argv, char **azColName)
{
    DataFrame *dataframe = static_cast<DataFrame *>(data);
    std::vector<any> row;

    for (int i = 0; i < argc; i++)
    {
        row.push_back(argv[i]);
    }

    dataframe->addRow(row);
    return 0;
}

DataFrame DatabaseExtractor::extractData()
{
    DataFrame dataframe;
    int rc;

    rc = sqlite3_exec(db, query.c_str(), callback, &dataframe, NULL);

    if (rc != SQLITE_OK)
    {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
    }

    return dataframe;
}