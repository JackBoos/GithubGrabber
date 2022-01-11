#include "dataAnalysis.h"
#include <json/json.h>

dataAnalysis* dataAnalysis::_ptr = nullptr;
dataAnalysis::dataAnalysis()
{
}

dataAnalysis::~dataAnalysis()
{
    if (_ptr)
        delete _ptr;
}

dataAnalysis& dataAnalysis::GetInstance()
{
    if (_ptr)
    {
        return *_ptr;
    }
    else
    {
        _ptr = new dataAnalysis;
    }
}

int dataAnalysis::GetTotalSize(const std::string& inString, const char* keyword)
{
    if (inString.empty() || !keyword || !(*keyword))
        return 0;

    Json::Reader reader;
    Json::Value root;

    int iCount = 0;

    if (reader.parse(inString, root))
    {
        iCount = root[keyword].asInt64();
    }
    return iCount;
}

bool dataAnalysis::ParseData(const std::string& inString, const char* subLable, IteamDataList& pOut)
{
    if (inString.empty() || !subLable)
        return false;

    Json::Reader reader;
    Json::Value root;
    Json::Value leaf;
    if (reader.parse(inString, root))
    {
        leaf = root[subLable];
        // Get first iteam's members
        auto names = leaf[0].getMemberNames();
        int nameCount = names.size();

        for (auto i = 0; i < leaf.size(); i++)
        {
            OneIteamDataList onelist;
            const auto& currentItem = leaf[i];
            for (size_t j = 0; j < nameCount; j++)
            {
                DataType datatype = DataType::DATATYPE_UNKNOWN;
                switch (currentItem[names[j]].type())
                {
                case Json::ValueType::nullValue:
                    break;
                case Json::ValueType::intValue:
                case Json::ValueType::uintValue:
                {
                    datatype = DataType::DATATYPE_INT;
                }
                    break;
                case Json::ValueType::stringValue:
                {
                    datatype = DataType::DATATYPE_STRING;
                }
                    break;
                default:
                    break;
                }

                if (datatype != DataType::DATATYPE_UNKNOWN)
                {
                    void* pData = nullptr;
                    switch (datatype)
                    {
                    case DataType::DATATYPE_INT:
                    {
                        pData = new int;
                        int& iData = *(int*)pData;
                        iData = currentItem[names[j]].asInt64();
                    }
                    break;
                    case DataType::DATATYPE_STRING:
                    {
                        pData = new std::string;
                        std::string& iData = *(std::string*)pData;
                        if (currentItem[names[j]].asCString() && *currentItem[names[j]].asCString())
                        {
                            iData = currentItem[names[j]].asCString();
                        }
                    }
                    break;
                    default:
                        break;
                    }
                    onelist.push_back(ItemData{ names[j] , datatype, pData });
                }
            }
            pOut.push_back(onelist);
        }
    }
}
