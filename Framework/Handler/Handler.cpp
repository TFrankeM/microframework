#include "Handler.h"
#include "../Dataframe/Series.h"
#include <any>
#include <vector>
#include <map>

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

MeasureHandler::MeasureHandler(Aggregation aggregation)
    : aggregation(aggregation) {}

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

GroupByHandler::GroupByHandler(const std::string &groupByColumn, Aggregation aggregation)
    : groupByColumn(groupByColumn), aggregation(aggregation) {}

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

JoinHandler::JoinHandler(const std::string &joinColumn)
    : joinColumn(joinColumn) {}

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

SortHandler::SortHandler(const std::string &sortColumn)
    : sortColumn(sortColumn) {}

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

    // Insertion sort
    for (int i = 0; i < input[0].columns[0].size(); i++)
    {
        std::vector<any> row = input[0].getRow(i);
        // Binary search to find the correct position
        int low = 0;
        int high = output.columns[0].size() - 1;
        while (low <= high)
        {
            int mid = low + (high - low) / 2;
            if (output.columns[columnIndex].get(mid) < row[columnIndex])
            {
                low = mid + 1;
            }
            else
            {
                high = mid - 1;
            }
        }

        output.insertRow(low, row);
    }

    vector<DataFrame> outputVector;
    outputVector.push_back(output);
    return outputVector;
}