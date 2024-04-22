#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <string>

class Repository {
private:
    std::string directoryPath;

public:
    Repository(const std::string& dirPath);

    DataFrame loadData_txt();
    //void saveData(DataFrame& dataframe);
};

#endif // REPOSITORY_H