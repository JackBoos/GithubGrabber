#pragma once
#include <string>
#include "commonData.h"
#include "urlParser.h"

class grabber
{
public:
    grabber(const char* baseUrl, const char* username = nullptr, const char* token = nullptr);
    ~grabber();

    bool Init();

    bool GetData(SearchCondition& search, const ConditionList& conditions, const char* outFile, unsigned int onePageCount);
private:
    bool bInit;
    std::string m_baseUrl;
    std::string m_usr;
    std::string m_token;
    urlParser* m_parser;
};
