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
    Handler(vector<Queue *> inputQueues, vector<Queue *> outputQueues);
    virtual void run();
    // Método para executar o manipulador em uma thread
    void runThreads(int numThreads);
};

class FilterHandler : public Handler
{
private:
    std::string filterColumn;
    std::string filterValue;

public:
    FilterHandler(vector<Queue *> inputQueues, vector<Queue *> outputQueues, const std::string &filterColumn, const std::string &filterValue);
    vector<DataFrame> process(const vector<DataFrame> &input) override;
};

enum class Aggregation
{
    SUM,
    AVG,
    MAX,
    MIN,
    COUNT
};

class MeasureHandler : public Handler
{
private:
    Aggregation aggregation;

public:
    MeasureHandler(vector<Queue *> inputQueues, vector<Queue *> outputQueues, Aggregation aggregation);
    vector<DataFrame> process(const vector<DataFrame> &input) override;
};

class GroupByHandler : public Handler
{
private:
    std::string groupByColumn;
    Aggregation aggregation;

public:
    GroupByHandler(vector<Queue *> inputQueues, vector<Queue *> outputQueues, const std::string &groupByColumn, Aggregation aggregation);
    vector<DataFrame> process(const vector<DataFrame> &input) override;
};

class JoinHandler : public Handler
{
private:
    std::string joinColumn;

public:
    JoinHandler(vector<Queue *> inputQueues, vector<Queue *> outputQueues, const std::string &joinColumn);
    vector<DataFrame> process(const vector<DataFrame> &input) override;
};

class ConcatHandler : public Handler
{
public:
    ConcatHandler(vector<Queue *> inputQueues, vector<Queue *> outputQueues);
    vector<DataFrame> process(const vector<DataFrame> &input) override;
};

enum class SortOrder
{
    ASC,
    DESC
};

class SortHandler : public Handler
{
private:
    std::string sortColumn;
    SortOrder sortOrder;

public:
    SortHandler(vector<Queue *> inputQueues, vector<Queue *> outputQueues, const std::string &sortColumn, SortOrder sortOrder);
    vector<DataFrame> process(const vector<DataFrame> &input) override;
};

class SplitHandler : public Handler
{
private:
    int splitSize;

public:
    SplitHandler(vector<Queue *> inputQueues, vector<Queue *> outputQueues, int splitSize);
    vector<DataFrame> process(const vector<DataFrame> &input) override;
};

class DropHandler : public Handler
{
private:
    vector<std::string> columns;

public:
    DropHandler(vector<Queue *> inputQueues, vector<Queue *> outputQueues, const vector<std::string> &columns);
    vector<DataFrame> process(const vector<DataFrame> &input) override;
};

class CopyHandler : public Handler
{
private:
    int numCopies;

public:
    CopyHandler(vector<Queue *> inputQueues, vector<Queue *> outputQueues, int numCopies);
    vector<DataFrame> process(const vector<DataFrame> &input) override;
};

#endif // HANDLER_H
