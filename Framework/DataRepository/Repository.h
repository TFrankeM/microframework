#ifndef REPOSITORY_H
#define REPOSITORY_H

#include "../Dataframe/Dataframe.h" // Include the header file of the DataFrame class
#include "../Dataframe/Series.h"
#include <string>

/**
 * @brief Class responsible for handling data loading and saving operations from a repository.
 */
class Repository {
private:
    std::string directoryPath; // Path of the directory where the data is stored

public:
    /**
     * @brief Constructor for the Repository class.
     * @param dirPath The path of the directory where the data is stored.
     */
    Repository(const std::string& dirPath);

    //Loads data from a text file and returns a DataFrame.
    DataFrame load_txt();

    //Loads data from a text file (http request simulation) and returns a DataFrame.
    DataFrame load_httpRequest();

    //Loads data from a csv file and returns a vector of DataFrames.
    std::vector<DataFrame> load_csv();

    //Print data from a DataFrame on the terminal
    void printDataFrame(DataFrame& dataframe);
};

#endif // REPOSITORY_H