#include "Repository.h"

void Repository::loadData()
{
    dataframe = extractor->extractData();
}

DataFrame Repository::getData()
{
    return dataframe;
}