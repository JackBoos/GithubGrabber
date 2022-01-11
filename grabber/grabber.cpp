#include "commonData.h"
#include "grabber.h"
#include "dataAnalysis.h"
#include "excelExecuter.h"
#include <string>

grabber::grabber(const char* baseUrl, const char* username, const char* token)
{
    bInit = false;
    if (baseUrl && *baseUrl)
        m_baseUrl = baseUrl;

    if (username && *username)
        m_usr = username;

    if (token && *token)
        m_token = token;

    m_parser = nullptr;
}

grabber::~grabber()
{
    if (m_parser)
        delete m_parser;
}

bool grabber::Init()
{
    if (m_baseUrl.empty())
        return false;

    m_parser = new urlParser(m_baseUrl.c_str());
    bInit = m_parser->Init(m_usr.c_str(), m_token.c_str());

    return bInit;
}

bool grabber::GetData(SearchCondition& search, const ConditionList& conditions, const char* outFile, unsigned int onePageCount)
{
    if (!bInit || !outFile || !(*outFile) || !onePageCount)
        return false;

    bool bSuc = false;

    std::string strSubUrl = search.name;
    strSubUrl += QUERY_KEYWORD;
    for (auto i = conditions.begin(); i != conditions.end(); i++)
    {
        if (!(i->empty()))
        {
            strSubUrl += *i;
            strSubUrl += " ";
        }
    }
    // Remove last empty space
    strSubUrl.erase(strSubUrl.length() - 1, 1);

    std::string outData;

    char szOnePageSize[8] = { 0 };
    sprintf(szOnePageSize, "%d", onePageCount);

    // Request first page to get the item count
    std::string strCurrentUrl = strSubUrl + PAGE_KEYWORD + "1" + PER_PARGE_KEYWORD + szOnePageSize;
    m_parser->GetData(strCurrentUrl.c_str(), outData);

    int iTotalItemSize = dataAnalysis::GetInstance().GetTotalSize(outData, GITHUB_TOTAL_COUNT_NAME);
    IteamDataList vData;
    dataAnalysis::GetInstance().ParseData(outData, GITHUB_ITEAM_NAME, vData);

    // Start from page 2
    const int getPage = ((int)((float)iTotalItemSize / (float)onePageCount) > (iTotalItemSize / onePageCount)) ? (iTotalItemSize / onePageCount) : (iTotalItemSize / onePageCount + 1);
    for (int i = 2; i <= getPage; i++)
    {
        char szPageNum[4] = { 0 };
        sprintf(szPageNum, "%d", i);
        outData.clear();
        strCurrentUrl = strSubUrl + PAGE_KEYWORD + szPageNum + PER_PARGE_KEYWORD + szOnePageSize;
        m_parser->GetData(strCurrentUrl.c_str(), outData);
        dataAnalysis::GetInstance().ParseData(outData, GITHUB_ITEAM_NAME, vData);
    }

    excelExecuter::GetInstance().WriteToFile(outFile, vData);

    // cleanup
    for (auto i = vData.begin(); i != vData.end(); i++)
    {
        for (auto j = i->begin(); j != i->end(); j++)
        {
            if (j->value)
            {
                delete j->value;
            }
        }
    }
    vData.swap(IteamDataList());

    return bSuc;
}
