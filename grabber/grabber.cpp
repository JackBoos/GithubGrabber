#include "commonData.h"
#include "grabber.h"
#include "dataAnalysis.h"
#include "excelExecuter.h"
#include <string>

using namespace GithubGrabber;

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

void grabber::ResetBaseUrl(const char* baseurl)
{
    if (!baseurl || !*baseurl)
        return;
    m_baseUrl = baseurl;
    m_parser->ResetBaseUrl(baseurl);
}

bool grabber::GetData(SearchCondition& search, const ConditionList& conditions, const char* outFile, unsigned int onePageCount, const ConditionList& filterConditions, IteamDataList* outDataList, bool bSave)
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
    bSuc = m_parser->GetData(strCurrentUrl.c_str(), outData);
    if (!bSuc)
        return bSuc;

    int iTotalItemSize = dataAnalysis::GetInstance().GetTotalSize(outData, GITHUB_TOTAL_COUNT_NAME);
    IteamDataList vData;
    bSuc = dataAnalysis::GetInstance().ParseData(outData, GITHUB_ITEAM_NAME, vData);
    if (!bSuc)
        return bSuc;

    // Start from page 2
    const int getPage = ((int)((float)iTotalItemSize / (float)onePageCount) > (iTotalItemSize / onePageCount)) ? (iTotalItemSize / onePageCount) : (iTotalItemSize / onePageCount + 1);
    for (int i = 2; i <= getPage; i++)
    {
        char szPageNum[4] = { 0 };
        sprintf(szPageNum, "%d", i);
        outData.clear();
        strCurrentUrl = strSubUrl + PAGE_KEYWORD + szPageNum + PER_PARGE_KEYWORD + szOnePageSize;
        m_parser->GetData(strCurrentUrl.c_str(), outData);
        bSuc = dataAnalysis::GetInstance().ParseData(outData, GITHUB_ITEAM_NAME, vData);
        if (!bSuc)
            return bSuc;
    }

    if (bSave)
        bSuc = excelExecuter::GetInstance().WriteToFile(outFile, vData, filterConditions);

    // cleanup
    if (outDataList)
    {
        vData.swap(*outDataList);
    }
    else
    {
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
    }

    return bSuc;
}

bool grabber::Execute(const std::string& repo, SearchType type, const ExecuteData& data)
{
    if (repo.empty() || type == SearchType::SEARCHTYPE_UNKNOWN || data.type == ExecuteType::EXECUTE_TYPE_UNKNOWN)
        return false;

    std::string subUrl = GITHUB_REPOS_NAME;
    subUrl += repo;
    switch (type)
    {
    case SearchType::SEARCHTYPE_ISSUE:
    {
        subUrl += GITHUB_ISSUES_NAME;
    }
    break;
    case SearchType::SEARCHTYPE_PR:
    {
        subUrl += GITHUB_PRS_NAME;
    }
    break;
    default:
        return false;
        break;
    }

    char szIssueId[16] = { 0 };
    itoa(data.issue_pr_id, szIssueId, 10);
    subUrl += szIssueId;

    switch (data.type)
    {
    case ExecuteType::EXECUTE_TYPE_ADD_COMMENT:
    {
        subUrl += GITHUB_COMMENTS_NAME;

        std::string strComment = GITHUB_BODY_START;
        if (data.dst != ExecuteOpDest::EXECUTE_OP_TO_NONE)
        {
            //strComment = "@" + data.dst + " ";
        }
        strComment += data.comment;
        strComment += GITHUB_BODY_END;
        return m_parser->PostData(subUrl.c_str(), strComment);
    }
    break;
    case ExecuteType::EXECUTE_TYPE_CLOSE_ISSUE_PR:
    {
        return m_parser->PostData(subUrl.c_str(), GITHUB_CLOSE_ISSUE_PR_BODY, GITHUB_OP_PATCH);
    }
    break;
    default:
        return false;
        break;
    }

    return true;
}