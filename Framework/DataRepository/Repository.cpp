#include "Repository.h"

void Repository::extractData()
{
    data = extractor->extractData();
}

vector<DataFrame> Repository::getData() const
{
    return data;
}