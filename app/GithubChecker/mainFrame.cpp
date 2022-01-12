#include "mainFrame.h"
#include <fstream>
#include <time.h>

#define ONE_MON_DAY 30
#define ONE_YEAR_DAY 365 * ONE_MON_DAY

TestFrame::TestFrame()
{
    m_editRepoUrl = nullptr;
    m_editUserName = nullptr;
    m_editToken = nullptr;
    m_editFilePath = nullptr;
    m_editFilterDay = nullptr;
    m_editComment = nullptr;
    m_editOutput = nullptr;
    m_optionFilterActiveDay = nullptr;
    m_optionFilterCreateDay = nullptr;
    m_optionIssue = nullptr;
    m_optionPR = nullptr;
    m_boxPortBug = nullptr;
    m_boxPortFeature = nullptr;
    m_boxQuestion = nullptr;
    m_boxNeedsRepro = nullptr;
    m_boxNeedMoreInfo = nullptr;
    m_boxDocument = nullptr;
    m_boxVcpkgBug = nullptr;
    m_boxVcpkgFeature = nullptr;

    m_gb = nullptr;
    m_iFilterDay = 0;
}

TestFrame::~TestFrame()
{
    if (m_gb)
        delete m_gb;
}

CDuiString TestFrame::GetSkinFolder()
{
    return _T("");
}

CDuiString TestFrame::GetSkinFile()
{
    return _T("checkerMainFrame.xml");
}

LPCTSTR TestFrame::GetWindowClassName(void) const
{
    return _T("MainFrame");
}

void TestFrame::InitWindow()
{
    m_editRepoUrl = (CEditUI*)m_PaintManager.FindControl(_T("REPO_NAME"));
    m_editUserName = (CEditUI*)m_PaintManager.FindControl(_T("USERNAME"));
    m_editToken = (CEditUI*)m_PaintManager.FindControl(_T("TOKEN"));
    m_editFilePath = (CEditUI*)m_PaintManager.FindControl(_T("FILE_PATH"));
    m_editFilterDay = (CEditUI*)m_PaintManager.FindControl(_T("FILTER_DAY_OVER"));
    m_editComment = (CRichEditUI*)m_PaintManager.FindControl(_T("COMMENT_CONTENT"));
    m_editOutput = (CRichEditUI*)m_PaintManager.FindControl(_T("OUTPUT"));
    m_optionFilterActiveDay = (COptionUI*)m_PaintManager.FindControl(_T("FILTER_ACTIVE_DAY"));
    m_optionFilterCreateDay = (COptionUI*)m_PaintManager.FindControl(_T("FILTER_CREATE_DAY"));
    m_optionIssue = (COptionUI*)m_PaintManager.FindControl(_T("SEARCH_TYPE_ISSUE"));
    m_optionPR = (COptionUI*)m_PaintManager.FindControl(_T("SEARCH_TYPE_PR"));
    m_boxPortBug = (CCheckBoxUI*)m_PaintManager.FindControl(_T("RULE_WITH_LABEL_PORT_BUG"));
    m_boxPortFeature = (CCheckBoxUI*)m_PaintManager.FindControl(_T("RULE_WITH_LABEL_PORT_FEATURE"));
    m_boxQuestion = (CCheckBoxUI*)m_PaintManager.FindControl(_T("RULE_WITH_LABEL_QUESTION"));
    m_boxNeedsRepro = (CCheckBoxUI*)m_PaintManager.FindControl(_T("RULE_WITH_LABEL_NEEDS_REPRO"));
    m_boxNeedMoreInfo = (CCheckBoxUI*)m_PaintManager.FindControl(_T("RULE_WITH_LABEL_NEED_MORE_INFO"));
    m_boxDocument = (CCheckBoxUI*)m_PaintManager.FindControl(_T("RULE_WITH_LABEL_DOCUMENT"));
    m_boxVcpkgBug = (CCheckBoxUI*)m_PaintManager.FindControl(_T("RULE_WITH_LABEL_VCPKG_BUG"));
    m_boxVcpkgFeature = (CCheckBoxUI*)m_PaintManager.FindControl(_T("RULE_WITH_LABEL_VCPKG_FEATURE"));

    std::wstring filepath = GetCurrentPath() + _T("result.xlsx");
    m_editFilePath->SetText(filepath.c_str());
}

LRESULT TestFrame::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    PostQuitMessage(0);
    return 0;
}

void TestFrame::Notify(TNotifyUI& msg)
{
    if (msg.sType == _T("click"))
    {
        if (msg.pSender->GetName() == _T("GET_DATA"))
        {
            OnGetData();
        }
        else if (msg.pSender->GetName() == _T("CLOSE"))
        {
            BOOL bTrue;
            OnClose(0, 0, 0, bTrue);
        }
        else if (msg.pSender->GetName() == _T("GET_FILE_PATH"))
        {
            OnGetFilePath();
        }
        else if (msg.pSender->GetName() == _T("GET_TOKEN"))
        {
            OnGetToken();
        }
        else if (msg.pSender->GetName() == _T("SEND_COMMENT"))
        {
            OnSendComment();
        }
        else if (msg.pSender->GetName() == _T("CLOSE_OUT_DATED"))
        {
            OnCloseOutdated();
        }
    }
}

void TestFrame::OnGetData()
{
    std::wstring repoName = (wchar_t*)m_editRepoUrl->GetText().GetData();
    char szRepoName[256];
    sprintf(szRepoName, "%ws", repoName.c_str());

    std::wstring userName = (wchar_t*)m_editUserName->GetText().GetData();
    char szUserName[256];
    sprintf(szUserName, "%ws", userName.c_str());

    std::wstring token = (wchar_t*)m_editToken->GetText().GetData();
    char szToken[256];
    sprintf(szToken, "%ws", token.c_str());

    std::wstring filePath = m_editFilePath->GetText().GetData();
    char szFilePath[256];
    sprintf(szFilePath, "%ws", filePath.c_str());

    std::wstring filterDay = m_editFilterDay->GetText().GetData();
    char szFilterDay[256];
    sprintf(szFilterDay, "%ws", filterDay.c_str());
    m_iFilterDay = atoi(szFilterDay);

    if (m_iFilterDay < 0)
    {
        ::MessageBox(NULL, _T("Please input a valid filter day number."), _T(""), NULL);
        return;
    }

    BOOL bIsIssue = m_optionIssue->IsSelected();
    SearchType search_type = bIsIssue ? SearchType::SEARCHTYPE_ISSUE : SearchType::SEARCHTYPE_PR;
    const char* search_type_message = bIsIssue ? SEARCHNAME_ISSUE : SEARCHNAME_PR;

    if (!(*szRepoName))
    {
        ::MessageBox(NULL, _T("Please input the repo name first."), _T(""), NULL);
        return;
    }

    if (!(*szFilePath))
    {
        ::MessageBox(NULL, _T("Please select the output file first."), _T(""), NULL);
        return;
    }

    if (!m_gb)
    {
        m_gb = new GithubGrabber::grabber(GITHUB_SEARCH_BASE_URL, szUserName, szToken);
        m_gb->Init();
    }
    m_gb->ResetBaseUrl(GITHUB_SEARCH_BASE_URL);

    ConditionList cdtList;
    std::string strRepo = "repo:";
    strRepo += szRepoName;
    cdtList.push_back(strRepo.c_str());
    if (bIsIssue)
        cdtList.push_back("type:issue");
    else
        cdtList.push_back("type:pr");

    if (m_boxPortBug->IsSelected())
        cdtList.push_back("label:\"category:port-bug\"");
    if (m_boxPortFeature->IsSelected())
        cdtList.push_back("label:\"category:port-feature\"");
    if (m_boxQuestion->IsSelected())
        cdtList.push_back("label:\"category:question\"");
    if (m_boxNeedsRepro->IsSelected())
        cdtList.push_back("label:\"requires:repro\"");
    if (m_boxNeedMoreInfo->IsSelected())
        cdtList.push_back("label:\"requires:more-information\"");
    if (m_boxDocument->IsSelected())
        cdtList.push_back("label:\"category:documentation\"");
    if (m_boxVcpkgBug->IsSelected())
        cdtList.push_back("label:\"category:vcpkg-bug\"");
    if (m_boxVcpkgFeature->IsSelected())
        cdtList.push_back("label:\"category:vcpkg-feature\"");

    cdtList.push_back("sort:created-desc");
    cdtList.push_back("state:open");

    if (!m_gb->GetData(SearchCondition{ search_type, search_type_message }, cdtList, szFilePath, 50, ConditionList(), &m_dataList, false))
    {
        ::MessageBox(NULL, _T("Failed to get data!"), _T(""), NULL);
        return;
    }

    FilterData();

    return;
}

void TestFrame::OnSendComment()
{
    std::wstring repoName = (wchar_t*)m_editRepoUrl->GetText().GetData();
    char szRepoName[256];
    sprintf(szRepoName, "%ws", repoName.c_str());

    std::wstring userName = (wchar_t*)m_editUserName->GetText().GetData();
    char szUserName[256];
    sprintf(szUserName, "%ws", userName.c_str());

    std::wstring token = (wchar_t*)m_editToken->GetText().GetData();
    char szToken[256];
    sprintf(szToken, "%ws", token.c_str());

    std::wstring commentContent = m_editComment->GetText().GetData();
    char szCommentContent[256];
    sprintf(szCommentContent, "%ws", commentContent.c_str());

    if (*szToken == '\0')
    {
        ::MessageBox(NULL, _T("Please input token first."), _T(""), NULL);
        return;
    }

    BOOL bIsIssue = m_optionIssue->IsSelected();
    SearchType search_type = bIsIssue ? SearchType::SEARCHTYPE_ISSUE : SearchType::SEARCHTYPE_PR;
    const char* search_type_message = bIsIssue ? SEARCHNAME_ISSUE : SEARCHNAME_PR;

    if (!(*szRepoName))
    {
        ::MessageBox(NULL, _T("Please input the repo name first."), _T(""), NULL);
        return;
    }

    if (!(*szCommentContent))
    {
        ::MessageBox(NULL, _T("Please type the comment."), _T(""), NULL);
        return;
    }

    if (!m_gb)
    {
        m_gb = new GithubGrabber::grabber(GITHUB_SEARCH_OPERATE_URL, szUserName, szToken);
        m_gb->Init();
    }
    m_gb->ResetBaseUrl(GITHUB_SEARCH_OPERATE_URL);

    ExecuteData data;
    data.type = ExecuteType::EXECUTE_TYPE_ADD_COMMENT;
    data.dst = ExecuteOpDest::EXECUTE_OP_TO_NONE;
    data.comment = szCommentContent;


    for (auto i = m_dataList.begin(); i != m_dataList.end(); i++)
    {
        data.issue_pr_id = *((int*)find(i->begin(), i->end(), "number")->value);
        if (!m_gb->Execute(szRepoName, search_type, data))
        {
            ::MessageBox(NULL, _T("Failed to execute operate!"), _T(""), NULL);
            return;
        }
    }

    ::MessageBox(NULL, _T("Execute operate success!"), _T(""), NULL);

    return;
}

void TestFrame::OnCloseOutdated()
{
    std::wstring repoName = (wchar_t*)m_editRepoUrl->GetText().GetData();
    char szRepoName[256];
    sprintf(szRepoName, "%ws", repoName.c_str());

    std::wstring userName = (wchar_t*)m_editUserName->GetText().GetData();
    char szUserName[256];
    sprintf(szUserName, "%ws", userName.c_str());

    std::wstring token = (wchar_t*)m_editToken->GetText().GetData();
    char szToken[256];
    sprintf(szToken, "%ws", token.c_str());

    if (*szToken == '\0')
    {
        ::MessageBox(NULL, _T("Please input token first."), _T(""), NULL);
        return;
    }

    BOOL bIsIssue = m_optionIssue->IsSelected();
    SearchType search_type = bIsIssue ? SearchType::SEARCHTYPE_ISSUE : SearchType::SEARCHTYPE_PR;
    const char* search_type_message = bIsIssue ? SEARCHNAME_ISSUE : SEARCHNAME_PR;

    if (!(*szRepoName))
    {
        ::MessageBox(NULL, _T("Please input the repo name first."), _T(""), NULL);
        return;
    }

    if (!m_gb)
    {
        m_gb = new GithubGrabber::grabber(GITHUB_SEARCH_OPERATE_URL, szUserName, szToken);
        m_gb->Init();
    }
    m_gb->ResetBaseUrl(GITHUB_SEARCH_OPERATE_URL);

    ExecuteData data;
    data.type = ExecuteType::EXECUTE_TYPE_CLOSE_ISSUE_PR;

    for (auto i = m_dataList.begin(); i != m_dataList.end(); i++)
    {
        data.issue_pr_id = *((int*)find(i->begin(), i->end(), "number")->value);
        if (!m_gb->Execute(szRepoName, search_type, data))
        {
            ::MessageBox(NULL, _T("Failed to execute operate!"), _T(""), NULL);
            return;
        }
    }

    ::MessageBox(NULL, _T("Execute operate success!"), _T(""), NULL);
    return;
}

void TestFrame::OnGetFilePath()
{
    OPENFILENAME ofn;
    TCHAR szFile[MAX_PATH] = _T("");

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = *this;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = _T(".xlsx");
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = GetCurrentPath().c_str();
    ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST;

    if (!GetOpenFileName(&ofn))
    {
        return;
    }

    if (wcscmp(szFile + wcslen(szFile) - wcslen(_T(".xlsx")), _T(".xlsx")))
        wcscat(szFile, _T(".xlsx"));
    m_editFilePath->SetText(szFile);
}

void TestFrame::OnGetToken()
{
    OPENFILENAME ofn;
    TCHAR szFile[MAX_PATH] = _T("");

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = *this;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = _T(".txt");
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = GetCurrentPath().c_str();
    ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (!GetOpenFileName(&ofn))
    {
        return;
    }

    char output[256];
    sprintf(output, "%ws", szFile);

    std::fstream fs;
    fs.open(output);

    if (!fs.is_open())
    {
        ::MessageBox(NULL, _T("Failed to open token file!"), _T(""), NULL);
        return;
    }

    std::string strToken;
    fs >> strToken;

    fs.close();

    if (strToken.empty())
    {
        ::MessageBox(NULL, _T("Token is empty!"), _T(""), NULL);
        return;
    }

    wchar_t woutput[1024];
    swprintf(woutput, 100, L"%hs", strToken.c_str());

    m_editToken->SetText(woutput);
}

void TestFrame::FilterData()
{
    if (m_dataList.empty())
        return;

    std::wstring desc = m_optionIssue->IsSelected() ? _T("issue") : _T("PR");
    bool bUseActiveDay = m_optionFilterActiveDay->IsSelected();

    std::wstring outData;
    for (auto i = m_dataList.begin(); i != m_dataList.end();)
    {
        if (i->empty())
            break;

        int idays = 0;
        if (bUseActiveDay)
        {
            idays = GetActiveDay(*(std::string*)find(i->begin(), i->end(), "updated_at")->value);
        }
        else
        {
            idays = GetActiveDay(*(std::string*)find(i->begin(), i->end(), "created_at")->value);
        }

        if (idays > m_iFilterDay)
        {
            auto tst = find(i->begin(), i->end(), "number")->value;
            wchar_t szOutDay[8] = { 0 };
            wchar_t szNumber[16] = { 0 };
            swprintf(szOutDay, 8, L"%d", idays);
            swprintf(szNumber, 16, L"%d", *(int*)find(i->begin(), i->end(), "number")->value);
            outData += desc;
            outData += _T(": ");
            outData += szNumber;
            outData += _T(" outdated ");
            outData += szOutDay;
            outData += _T(" days\n");
            i++;
        }
        else
        {
            i = m_dataList.erase(i);
        }
    }

    if (!m_dataList.empty())
        m_editOutput->SetText(outData.c_str());
}

std::wstring TestFrame::GetCurrentPath()
{
    TCHAR szFilePath[MAX_PATH + 1];

    GetModuleFileName(NULL, szFilePath, MAX_PATH);

    (_tcsrchr(szFilePath, _T('\\')))[1] = 0;

    return std::wstring(szFilePath);
}

int TestFrame::GetActiveDay(std::string& from)
{
    time_t now = time(NULL);
    struct tm *nowTime = localtime(&now);

    tm tmFromTime;

    int* date[6] = { &(tmFromTime.tm_year), &(tmFromTime.tm_mon), &(tmFromTime.tm_mday), &(tmFromTime.tm_hour), &(tmFromTime.tm_min), &(tmFromTime.tm_sec) };
    int i = 0;
    const char* begin = from.c_str();
    const char* curr = begin;
    while (*curr != '\0')
    {
        if (*curr < '0' || *curr > '9')
        {
            std::string strTmp;
            strTmp.insert(0, begin, curr - begin);
            *date[i] = atoi(strTmp.c_str());

            begin = curr + 1;
            i++;
        }
        curr++;
    }

    tmFromTime.tm_year -= 1900;
    tmFromTime.tm_mon--;

    bool bOverMon = tmFromTime.tm_mon > nowTime->tm_mon;
    bool bOverDay = tmFromTime.tm_mday > nowTime->tm_mday;
    int abs = ((int)(nowTime->tm_year - tmFromTime.tm_year - (bOverMon ? 1 : 0))) * ONE_YEAR_DAY
        + ((int)(nowTime->tm_mon - tmFromTime.tm_mon + (bOverMon ? 12 : 0) - (bOverDay ? 1 : 0))) * ONE_MON_DAY
        + (int)(nowTime->tm_mday - tmFromTime.tm_mday + (bOverDay ? 30 : 0));

    return abs;
}