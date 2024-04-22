#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <string>
#include "../Dataframe/Dataframe.h"
#include "Extractor.h"

class Repository {
private:
    Extractor* extractor;
    DataFrame dataframe;

public:
    Repository(Extractor* extractor) : extractor(extractor) {};

    void loadData();
    DataFrame getData();
};

#endif // REPOSITORY_H