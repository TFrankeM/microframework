#ifndef REPOSITORY_H
#define REPOSITORY_H

#include "../Dataframe/Dataframe.h" // Include the header file of the DataFrame class
#include "Extractor.h" // Include the header file of the Extractor class

/**
 * @brief Class responsible for managing data extraction and storage.
 */
class Repository
{
private:
    Extractor *extractor; // Pointer to an Extractor object
    DataFrame data; // DataFrame object to store the extracted data

public:
    /**
     * @brief Constructor for the Repository class.
     * 
     * @param extractor Pointer to an Extractor object.
     */
    Repository(Extractor *extractor) : extractor(extractor) {}

    /**
     * @brief Extracts data using the provided Extractor object.
     */
    void extractData();

    /**
     * @brief Returns the extracted data.
     * 
     * @return DataFrame The extracted data.
     */
    DataFrame getData() const;
};

#endif // REPOSITORY_H