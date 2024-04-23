#include "Repository.h"

void Repository::extractData()
{
    data = extractor->extractData();
}

DataFrame Repository::getData() const
{
    return data;
}