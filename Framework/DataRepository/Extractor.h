#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#include <string>
#include <sqlite3.h>
#include <mutex>
#include "../Dataframe/Dataframe.h"

using std::string;

class Extractor
{
public:
    virtual DataFrame extractData() = 0;
};

class FileExtractor : public Extractor
{
protected:
    string path;
    string separator;

public:
    FileExtractor(const string &filePath, const string &separator = ",") : path(filePath), separator(separator) {}

    DataFrame extractFromFile(const std::string &filePath);
    DataFrame extractData() override;
};

class DirectoryExtractor : public FileExtractor
{
private:
    string extension;

public:
    DirectoryExtractor(const std::string &dirPath, const std::string &separator = ",", const std::string &extension = ".csv")
        : FileExtractor(dirPath, separator), extension(extension) {}

    DataFrame extractData() override;
};

class RequestExtractor : public Extractor
{
private:
    DataFrame data;
    std::mutex dataMutex;

public:
    RequestExtractor() {}
    bool handleRequest(const std::string &request);
    DataFrame extractData() override;
};

class DatabaseExtractor : public Extractor
{
private:
    sqlite3 *db;
    std::string query;

public:
    DatabaseExtractor(sqlite3 *db, const std::string &query) : db(db), query(query) {}

    static int callback(void *data, int argc, char **argv, char **azColName);
    DataFrame extractData() override;
};

#endif // EXTRACTOR_H