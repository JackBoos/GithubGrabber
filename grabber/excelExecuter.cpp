#include "excelExecuter.h"
#include <xlsxio_version.h>
#include <xlsxio_write.h>
#include <vector>

#define DEFAULT_LABEL_NAME "data"
const int DEFAULT_COLUMN_LENGTH = 10;

excelExecuter* excelExecuter::_ptr = nullptr;

excelExecuter::excelExecuter()
{
}

excelExecuter::~excelExecuter()
{
    if (_ptr)
        delete _ptr;
}

excelExecuter& excelExecuter::GetInstance()
{
    if (_ptr)
    {
        return *_ptr;
    }
    else
    {
        _ptr = new excelExecuter;
    }
}

bool excelExecuter::WriteToFile(const char* filePath, const IteamDataList& dataList, const ConditionList& filterConditions)
{
    if (!filePath || !(*filePath))
        return false;

    if (dataList.empty())
        return true;

    std::string filename = filePath;
    std::string str_tmp = "";

    xlsxiowriter writer = nullptr;
    if ((writer = xlsxiowrite_open(filename.c_str(), DEFAULT_LABEL_NAME)) == nullptr)
    {
        return false;
    }

    std::vector<int> vPosition;
    // Writer row names
    const auto& items = dataList.begin();
    int pos = 0;
    for (auto i = items->begin(); i != items->end(); i++, pos++)
    {
        auto found = find(filterConditions.begin(), filterConditions.end(), i->name);
        if (filterConditions.empty() || found != filterConditions.end())
        {
            if (!filterConditions.empty())
                vPosition.push_back(pos);
            xlsxiowrite_add_column(writer, i->name.c_str(), DEFAULT_COLUMN_LENGTH);
        }
    }
    xlsxiowrite_next_row(writer);
    // Start to write data
    for (auto i = dataList.begin(); i != dataList.end(); i++)
    {
        const auto& currItem = i;
        int currPos = 0;
        for (auto j = currItem->begin(); j != currItem->end(); j++, currPos++)
        {
            if (find(vPosition.begin(), vPosition.end(), currPos) != vPosition.end())
            {
                switch (j->type)
                {
                case DataType::DATATYPE_INT:
                {
                    const int data = *(int*)(j->value);
                    xlsxiowrite_add_cell_int(writer, data);
                }
                break;
                case DataType::DATATYPE_STRING:
                {
                    const std::string& data = *(std::string*)(j->value);
                    if (data.length())
                        xlsxiowrite_add_cell_string(writer, data.c_str());
                }
                break;
                default:
                    break;
                }
            }
        }
        xlsxiowrite_next_row(writer);
    }

    xlsxiowrite_close(writer);

    return true;
}