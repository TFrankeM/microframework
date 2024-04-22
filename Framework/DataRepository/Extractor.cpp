#include "Extractor.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <map>
#include <sqlite3.h>

namespace fs = std::filesystem;
using std::string;

DataFrame FileExtractor::extractFromFile(const string &filePath)
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

DataFrame FileExtractor::extractData()
{
    return extractFromFile(path);
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

        DataFrame fileData = extractFromFile(entry.path().string());

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