#include "Handler.h"
#include "../Dataframe/Series.h"
#include <any>
#include <vector>
#include <map>
#include <cmath>
#include <thread>

using std::thread;

Handler::Handler(vector<Queue *> inputQueues, vector<Queue *> outputQueues)
    : inputQueues(inputQueues), outputQueues(outputQueues) {}

void Handler::run()
{
    while (true)
    {
        vector<DataFrame> input;
        for (const auto &queue : inputQueues)
        {
            input.push_back(queue->dequeue());
        }

        vector<DataFrame> output = process(input);

        for (int i = 0; i < output.size(); i++)
        {
            outputQueues[i]->enqueue(output[i]);
        }
    }
}

void Handler::runThreads(int numThreads)
{
    vector<thread> threads;
    for (int i = 0; i < numThreads; i++)
    {
        threads.push_back(thread([this]()
                                 { this->run(); }));
    }
}

FilterHandler::FilterHandler(vector<Queue *> inputQueues, vector<Queue *> outputQueues, const std::string &filterColumn, const std::string &filterValue)
    : Handler(inputQueues, outputQueues), filterColumn(filterColumn), filterValue(filterValue) {}

vector<DataFrame> FilterHandler::process(const vector<DataFrame> &input)
{
    DataFrame output;
    int columnIndex = -1;
    for (int i = 0; i < input[0].columnNames.size(); i++)
    {
        if (input[0].columnNames[i] == filterColumn)
        {
            columnIndex = i;
            break;
        }
    }

    for (const auto &df : input)
    {
        for (int i = 0; i < df.columns[0].size(); i++)
        {
            std::vector<any> row = df.getRow(i);
            if (std::any_cast<std::string>(df.getRow(i)[columnIndex]) == filterValue)
            {
                output.addRow(row);
                break;
            }
        }
    }

    vector<DataFrame> outputVector;
    outputVector.push_back(output);
    return outputVector;
}

any sum(const any &a, const any &b)
{
    if (std::holds_alternative<int>(a) && std::holds_alternative<int>(b))
    {
        return std::any_cast<int>(a) + std::any_cast<int>(b);
    }
    else if (std::holds_alternative<float>(a) && std::holds_alternative<float>(b))
    {
        return std::any_cast<float>(a) + std::any_cast<float>(b);
    }
    else if (std::holds_alternative<double>(a) && std::holds_alternative<double>(b))
    {
        return std::any_cast<double>(a) + std::any_cast<double>(b);
    }

    return 0;
}

MeasureHandler::MeasureHandler(vector<Queue *> inputQueues, vector<Queue *> outputQueues, Aggregation aggregation)
    : Handler(inputQueues, outputQueues), aggregation(aggregation) {}

vector<DataFrame> MeasureHandler::process(const vector<DataFrame> &input)
{
    DataFrame output;

    if (aggregation == Aggregation::COUNT)
    {
        output.columnNames.push_back("count");
        output.columns.push_back(Series());
        output.columns[0].add(static_cast<int>(input[0].columns[0].size()));

        vector<DataFrame> outputVector;
        outputVector.push_back(output);
        return outputVector;
    }

    output.columnNames = input[0].columnNames;
    for (int i = 0; i < input[0].columnNames.size(); i++)
    {
        output.columns.push_back(Series());
    }

    vector<any> row;
    for (int i = 0; i < input[0].columnNames.size(); i++)
    {
        any measure = input[0].columns[i].get(0);
        for (int j = 1; j < input[0].columns[0].size(); j++)
        {
            switch (aggregation)
            {
            case Aggregation::SUM:
            case Aggregation::AVG:
                measure = sum(measure, input[0].columns[i].get(j));
                break;
            case Aggregation::MAX:
                if (measure < input[0].columns[i].get(j))
                {
                    measure = input[0].columns[i].get(j);
                }
                break;
            case Aggregation::MIN:
                if (measure > input[0].columns[i].get(j))
                {
                    measure = input[0].columns[i].get(j);
                }
                break;
            default:
                break;
            }
        }

        if (aggregation == Aggregation::AVG)
        {
            measure = std::any_cast<double>(measure) / input[0].columns[0].size();
        }

        row.push_back(measure);
    }

    output.addRow(row);

    vector<DataFrame> outputVector;
    outputVector.push_back(output);
    return outputVector;
}

GroupByHandler::GroupByHandler(vector<Queue *> inputQueues, vector<Queue *> outputQueues, const std::string &groupByColumn, Aggregation aggregation)
    : Handler(inputQueues, outputQueues), groupByColumn(groupByColumn), aggregation(aggregation) {}

vector<DataFrame> GroupByHandler::process(const vector<DataFrame> &input)
{
    DataFrame output;
    if (aggregation == Aggregation::COUNT)
    {
        output.columnNames.push_back(groupByColumn);
        output.columnNames.push_back("count");
        output.columns.push_back(Series());
        output.columns.push_back(Series());
    }
    else
    {
        output.columnNames = input[0].columnNames;
        for (int i = 0; i < input[0].columnNames.size(); i++)
        {
            output.columns.push_back(Series());
        }
    }

    int columnIndex = -1;
    for (int i = 0; i < input[0].columnNames.size(); i++)
    {
        if (input[0].columnNames[i] == groupByColumn)
        {
            columnIndex = i;
            break;
        }
    }

    std::map<any, std::vector<any>> groups;
    std::map<any, int> groupCounts;
    for (int i = 0; i < input[0].columns[0].size(); i++)
    {
        std::vector<any> row = input[0].getRow(i);
        any groupByValue = row[columnIndex];
        if (groups.find(groupByValue) == groups.end())
        {
            groups[groupByValue] = std::vector<any>(output.columnNames.size(), 0);
            groupCounts[groupByValue] = 0;
        }

        for (int j = 0; j < row.size(); j++)
        {
            if (j == columnIndex)
            {
                continue;
            }

            switch (aggregation)
            {
            case Aggregation::AVG:
                groups[groupByValue][j] = sum(groups[groupByValue][j], row[j]);
                groupCounts[groupByValue]++;
                break;
            case Aggregation::SUM:
                groups[groupByValue][j] = sum(groups[groupByValue][j], row[j]);
                break;
            case Aggregation::MAX:
                if (groups[groupByValue][j] < row[j])
                {
                    groups[groupByValue][j] = row[j];
                }
                break;
            case Aggregation::MIN:
                if (groups[groupByValue][j] > row[j])
                {
                    groups[groupByValue][j] = row[j];
                }
                break;
            case Aggregation::COUNT:
                groupCounts[groupByValue]++;
                break;

            default:
                break;
            }
        }
    }

    for (const auto &group : groups)
    {
        if (aggregation == Aggregation::COUNT)
        {
            output.columns[0].add(group.first);
            output.columns[1].add(groupCounts[group.first]);
            continue;
        }

        for (int i = 0; i < group.second.size(); i++)
        {
            if (aggregation == Aggregation::AVG)
            {
                output.columns[i].add(std::any_cast<double>(group.second[i]) / groupCounts[group.first]);
            }
            else
            {
                output.columns[i].add(group.second[i]);
            }
        }
    }

    vector<DataFrame> outputVector;
    outputVector.push_back(output);
    return outputVector;
}

JoinHandler::JoinHandler(vector<Queue *> inputQueues, vector<Queue *> outputQueues, const std::string &joinColumn)
    : Handler(inputQueues, outputQueues), joinColumn(joinColumn) {}

vector<DataFrame> JoinHandler::process(const vector<DataFrame> &input)
{
    DataFrame output;
    int columnIndex = -1;
    for (int i = 0; i < input[0].columnNames.size(); i++)
    {
        if (input[0].columnNames[i] == joinColumn)
        {
            columnIndex = i;
            break;
        }
    }

    for (const auto &df : input)
    {
        for (int i = 0; i < df.columns[0].size(); i++)
        {
            std::vector<any> row = df.getRow(i);
            output.addRow(row);
        }
    }

    vector<DataFrame> outputVector;
    outputVector.push_back(output);
    return outputVector;
}

vector<DataFrame> ConcatHandler::process(const vector<DataFrame> &input)
{
    DataFrame output;
    for (const auto &df : input)
    {
        for (int i = 0; i < df.columns[0].size(); i++)
        {
            std::vector<any> row = df.getRow(i);
            output.addRow(row);
        }
    }

    vector<DataFrame> outputVector;
    outputVector.push_back(output);
    return outputVector;
}

SortHandler::SortHandler(vector<Queue *> inputQueues, vector<Queue *> outputQueues, const std::string &sortColumn, SortOrder sortOrder)
    : Handler(inputQueues, outputQueues), sortColumn(sortColumn), sortOrder(sortOrder) {}

vector<DataFrame> SortHandler::process(const vector<DataFrame> &input)
{
    DataFrame output;
    int columnIndex = -1;
    for (int i = 0; i < input[0].columnNames.size(); i++)
    {
        if (input[0].columnNames[i] == sortColumn)
        {
            columnIndex = i;
            break;
        }
    }

    std::vector<std::vector<any>> rows;
    for (const auto &df : input)
    {
        for (int i = 0; i < df.columns[0].size(); i++)
        {
            std::vector<any> row = df.getRow(i);
            rows.push_back(row);
        }
    }

    std::sort(rows.begin(), rows.end(), [columnIndex, this](const std::vector<any> &a, const std::vector<any> &b)
              {
        if (sortOrder == SortOrder::ASC)
        {
            return a[columnIndex] < b[columnIndex];
        }
        return a[columnIndex] > b[columnIndex]; });

    for (const auto &row : rows)
    {
        output.addRow(row);
    }

    vector<DataFrame> outputVector;
    outputVector.push_back(output);
    return outputVector;
}

SplitHandler::SplitHandler(vector<Queue *> inputQueues, vector<Queue *> outputQueues, int splitSize)
    : Handler(inputQueues, outputQueues), splitSize(splitSize) {}

vector<DataFrame> SplitHandler::process(const vector<DataFrame> &input)
{
    vector<DataFrame> output;
    for (const auto &df : input)
    {
        for (int i = 0; i < df.columns[0].size(); i += splitSize)
        {
            DataFrame split;
            for (int j = i; j < i + splitSize && j < df.columns[0].size(); j++)
            {
                split.addRow(df.getRow(j));
            }
            output.push_back(split);
        }
    }

    return output;
}

DropHandler::DropHandler(vector<Queue *> inputQueues, vector<Queue *> outputQueues, const vector<std::string> &columns)
    : Handler(inputQueues, outputQueues), columns(columns) {}

vector<DataFrame> DropHandler::process(const vector<DataFrame> &input)
{
    DataFrame output;
    // Add columns that are not in the list of columns to drop
    for (int i = 0; i < input[0].columnNames.size(); i++)
    {
        if (std::find(columns.begin(), columns.end(), input[0].columnNames[i]) == columns.end())
        {
            output.columnNames.push_back(input[0].columnNames[i]);
            output.columns.push_back(Series());
        }
    }

    for (const auto &df : input)
    {
        for (int i = 0; i < df.columns[0].size(); i++)
        {
            std::vector<any> row = df.getRow(i);
            for (int j = 0; j < row.size(); j++)
            {
                if (std::find(columns.begin(), columns.end(), df.columnNames[j]) == columns.end())
                {
                    output.addRow(row);
                    break;
                }
            }
        }
    }

    vector<DataFrame> outputVector;
    outputVector.push_back(output);
    return outputVector;
}

CopyHandler::CopyHandler(vector<Queue *> inputQueues, vector<Queue *> outputQueues, int numCopies)
    : Handler(inputQueues, outputQueues), numCopies(numCopies) {}

vector<DataFrame> CopyHandler::process(const vector<DataFrame> &input)
{
    vector<DataFrame> output;
    for (int i = 0; i < numCopies; i++)
    {
        DataFrame copy;
        for (const auto &df : input)
        {
            for (int j = 0; j < df.columns[0].size(); j++)
            {
                copy.addRow(df.getRow(j));
            }
        }
        output.push_back(copy);
    }

    return output;
}