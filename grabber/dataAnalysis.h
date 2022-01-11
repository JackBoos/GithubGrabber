#pragma once
#include <string>
#include "commonData.h"

class dataAnalysis
{
public:
    dataAnalysis();
    ~dataAnalysis();

    static dataAnalysis& GetInstance();

    int GetTotalSize(const std::string& inString, const char* keyword);
    bool ParseData(const std::string& inString, const char* subLable, IteamDataList& pOut);
private:
    static dataAnalysis* _ptr;
    std::string* pString;
};
