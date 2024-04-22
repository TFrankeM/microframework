#ifndef HANDLER_H
#define HANDLER_H

#include <vector>
#include "../Queue/Queue.h"
#include "../Dataframe/Dataframe.h"

using std::vector;

class Handler
{
protected:
    vector<Queue *> inputQueues;
    vector<Queue *> outputQueues;

    // Método virtual puro para processar um DataFrame de entrada e produzir um ou mais DataFrames como saída
    virtual vector<DataFrame> process(const vector<DataFrame> &input) = 0;

public:
    // Método para executar o manipulador em uma thread
    virtual void runThread() = 0;
};

class FilterHandler : public Handler
{
private:
    std::string filterColumn;
    std::string filterValue;

public:
    FilterHandler(const std::string &filterColumn, const std::string &filterValue);
    vector<DataFrame> process(const vector<DataFrame> &input) override;
};

enum class Aggregation
{
    SUM,
    AVG
};

class GroupByHandler : public Handler
{
private:
    std::string groupByColumn;
    Aggregation aggregation;

public:
    GroupByHandler(const std::string &groupByColumn, Aggregation aggregation);
    vector<DataFrame> process(const vector<DataFrame> &input) override;
};

class JoinHandler : public Handler
{
private:
    std::string joinColumn;

public:
    JoinHandler(const std::string &joinColumn);
    vector<DataFrame> process(const vector<DataFrame> &input) override;
};

class CountHandler : public Handler
{
public:
    vector<DataFrame> process(const vector<DataFrame> &input) override;
};

#endif // HANDLER_H
