#include "Handler.h"
#include <any>
#include <vector>

FilterHandler::FilterHandler(const std::string &filterColumn, const std::string &filterValue)
    : filterColumn(filterColumn), filterValue(filterValue) {}

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

GroupByHandler::GroupByHandler(const std::string &groupByColumn, Aggregation aggregation)
    : groupByColumn(groupByColumn), aggregation(aggregation) {}

vector<DataFrame> GroupByHandler::process(const vector<DataFrame> &input) {
    DataFrame output;
    int columnIndex = -1;
    for (int i = 0; i < input[0].columnNames.size(); i++)
    {
        if (input[0].columnNames[i] == groupByColumn)
        {
            columnIndex = i;
            break;
        }
    }

    std::vector<std::string> uniqueValues;
    for (const auto &df : input)
    {
        for (int i = 0; i < df.columns[0].size(); i++)
        {
            std::string value = std::any_cast<std::string>(df.getRow(i)[columnIndex]);
            if (std::find(uniqueValues.begin(), uniqueValues.end(), value) == uniqueValues.end())
            {
                uniqueValues.push_back(value);
            }
        }
    }

    for (const auto &value : uniqueValues)
    {
        std::vector<any> row;
        row.push_back(value);
        for (int i = 1; i < input[0].columnNames.size(); i++)
        {
            if (i == columnIndex)
            {
                continue;
            }

            double sum = 0;
            int count = 0;
            for (const auto &df : input)
            {
                for (int j = 0; j < df.columns[0].size(); j++)
                {
                    if (std::any_cast<std::string>(df.getRow(j)[columnIndex]) == value)
                    {
                        sum += std::any_cast<double>(df.getRow(j)[i]);
                        count++;
                    }
                }
            }

            if (aggregation == Aggregation::SUM)
            {
                row.push_back(sum);
            }
            else if (aggregation == Aggregation::AVG)
            {
                row.push_back(sum / count);
            }
        }
        output.addRow(row);
    }

    vector<DataFrame> outputVector;
    outputVector.push_back(output);
    return outputVector;
}

JoinHandler::JoinHandler(const std::string &joinColumn)
    : joinColumn(joinColumn) {}

vector<DataFrame> JoinHandler::process(const vector<DataFrame> &input) {
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

vector<DataFrame> CountHandler::process(const vector<DataFrame> &input) {
    DataFrame output;
    std::vector<std::string> uniqueValues;
    for (const auto &df : input)
    {
        for (int i = 0; i < df.columns[0].size(); i++)
        {
            std::string value = std::any_cast<std::string>(df.getRow(i)[0]);
            if (std::find(uniqueValues.begin(), uniqueValues.end(), value) == uniqueValues.end())
            {
                uniqueValues.push_back(value);
            }
        }
    }

    std::vector<any> row;
    row.push_back("Count");
    row.push_back(static_cast<int>(uniqueValues.size()));
    output.addRow(row);

    vector<DataFrame> outputVector;
    outputVector.push_back(output);
    return outputVector;
}