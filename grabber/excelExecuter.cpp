#include "excelExecuter.h"
#include <xlsxio_version.h>
#include <xlsxio_write.h>

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

bool excelExecuter::WriteToFile(const char* filePath, const IteamDataList& dataList)
{
    if (!filePath || !(*filePath))
        return false;

    if (dataList.empty())
        return true;

    std::string filename = filePath;
    std::string str_tmp = "";

    xlsxiowriter writer = nullptr;
    if ((writer = xlsxiowrite_open(filePath, DEFAULT_LABEL_NAME)) == nullptr)
    {
        return false;
    }

    // Writer row names
    const auto& items = dataList.begin();
    for (auto i = items->begin(); i != items->end(); i++)
    {
        xlsxiowrite_add_column(writer, i->name.c_str(), DEFAULT_COLUMN_LENGTH);
    }
    xlsxiowrite_next_row(writer);
    // Start to write data
    for (auto i = dataList.begin(); i != dataList.end(); i++)
    {
        const auto& currItem = i;
        for (auto j = currItem->begin(); j != currItem->end(); j++)
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
        xlsxiowrite_next_row(writer);
    }
    xlsxiowrite_next_row(writer);

    xlsxiowrite_close(writer);

    return true;
}