#include "mainFrame.h"
#include "grabber.h"
#include <fstream>

TestFrame::TestFrame()
{
    m_editBaseUrl = nullptr;
    m_editRepoUrl = nullptr;
    m_editSearchUrl = nullptr;
    m_editUserName = nullptr;
    m_editToken = nullptr;
    m_editFilePath = nullptr;
    m_optionIssue = nullptr;
    m_optionPR = nullptr;
    m_boxOpened = nullptr;
    m_boxClosed = nullptr;
    m_boxPortBug = nullptr;
    m_boxPortFeature = nullptr;
    m_boxQuestion = nullptr;
    m_boxNeedsRepro = nullptr;
    m_boxNeedMoreInfo = nullptr;
    m_boxDocument = nullptr;
    m_boxVcpkgBug = nullptr;
    m_boxVcpkgFeature = nullptr;
    m_optionSortLower = nullptr;
    m_optionSortHigher = nullptr;
}

TestFrame::~TestFrame()
{

}

CDuiString TestFrame::GetSkinFolder()
{
    return _T("");
}

CDuiString TestFrame::GetSkinFile()
{
    return _T("mainFrame.xml");
}

LPCTSTR TestFrame::GetWindowClassName(void) const
{
    return _T("MainFrame");
}

void TestFrame::InitWindow()
{
    m_editBaseUrl = (CEditUI*)m_PaintManager.FindControl(_T("BASE_URL"));
    m_editRepoUrl = (CEditUI*)m_PaintManager.FindControl(_T("REPO_NAME"));
    m_editUserName = (CEditUI*)m_PaintManager.FindControl(_T("USERNAME"));
    m_editToken = (CEditUI*)m_PaintManager.FindControl(_T("TOKEN"));
    m_editFilePath = (CEditUI*)m_PaintManager.FindControl(_T("FILE_PATH"));
    m_editFilterPath = (CEditUI*)m_PaintManager.FindControl(_T("FILTER_RULES"));
    m_optionIssue = (COptionUI*)m_PaintManager.FindControl(_T("SEARCH_TYPE_ISSUE"));
    m_optionPR = (COptionUI*)m_PaintManager.FindControl(_T("SEARCH_TYPE_PR"));
    m_boxOpened = (CCheckBoxUI*)m_PaintManager.FindControl(_T("RULE_WITH_OPENED"));
    m_boxClosed = (CCheckBoxUI*)m_PaintManager.FindControl(_T("RULE_WITH_CLOSED"));
    m_boxPortBug = (CCheckBoxUI*)m_PaintManager.FindControl(_T("RULE_WITH_LABEL_PORT_BUG"));
    m_boxPortFeature = (CCheckBoxUI*)m_PaintManager.FindControl(_T("RULE_WITH_LABEL_PORT_FEATURE"));
    m_boxQuestion = (CCheckBoxUI*)m_PaintManager.FindControl(_T("RULE_WITH_LABEL_QUESTION"));
    m_boxNeedsRepro = (CCheckBoxUI*)m_PaintManager.FindControl(_T("RULE_WITH_LABEL_NEEDS_REPRO"));
    m_boxNeedMoreInfo = (CCheckBoxUI*)m_PaintManager.FindControl(_T("RULE_WITH_LABEL_NEED_MORE_INFO"));
    m_boxDocument = (CCheckBoxUI*)m_PaintManager.FindControl(_T("RULE_WITH_LABEL_DOCUMENT"));
    m_boxVcpkgBug = (CCheckBoxUI*)m_PaintManager.FindControl(_T("RULE_WITH_LABEL_VCPKG_BUG"));
    m_boxVcpkgFeature = (CCheckBoxUI*)m_PaintManager.FindControl(_T("RULE_WITH_LABEL_VCPKG_FEATURE"));
    m_optionSortLower = (COptionUI*)m_PaintManager.FindControl(_T("SORT_WITH_CERATE_TIME_LOWER"));
    m_optionSortHigher = (COptionUI*)m_PaintManager.FindControl(_T("SORT_WITH_CERATE_TIME_HIGHER"));

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
        else if (msg.pSender->GetName() == _T("GET_RULE"))
        {
            OnGetRule();
        }
        else if (msg.pSender->GetName() == _T("GET_TOKEN"))
        {
            OnGetToken();
        }
        else if (msg.pSender->GetName() == _T("GET_FILTER_RULES"))
        {
            OnGetFilters();
        }
    }
}

void TestFrame::OnGetData()
{
    std::wstring baseUrl = (wchar_t*)m_editBaseUrl->GetText().GetData();
    char szBaseUrl[256];
    sprintf(szBaseUrl, "%ws", baseUrl.c_str());

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


    ConditionList filterConditions;
    GetFilterConditions(filterConditions);

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

    grabber gb(szBaseUrl, szUserName, szToken);
    gb.Init();

    ConditionList cdtList;
    std::string strRepo = "repo:";
    strRepo += szRepoName;
    cdtList.push_back(strRepo.c_str());
    if (bIsIssue)
        cdtList.push_back("type:issue");
    else
        cdtList.push_back("type:pr");

    if (m_boxOpened->IsSelected())
        cdtList.push_back("state:open");
    if (m_boxClosed->IsSelected())
        cdtList.push_back("state:close");

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

    if (m_optionSortLower->IsSelected())
        cdtList.push_back("sort:created-desc");
    else
        cdtList.push_back("sort:created-asc");

    if (gb.GetData(SearchCondition{ search_type, search_type_message }, cdtList, szFilePath, 50, filterConditions))
    {
        ::MessageBox(NULL, _T("Download data success!"), _T(""), NULL);
    }
    else
    {
        ::MessageBox(NULL, _T("Failed to get data!"), _T(""), NULL);
    }
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

void TestFrame::GetFilterConditions(ConditionList& conditions)
{
    std::wstring filterCondition = m_editFilterPath->GetText().GetData();
    if (filterCondition.empty())
        return;

    char* szFilterCondition = new char[filterCondition.length() + 1];
    sprintf(szFilterCondition, "%ws", filterCondition.c_str());

    char* startPtr = szFilterCondition;
    char* currPtr = startPtr;
    while (*currPtr != '\0')
    {
        if (*currPtr == ';')
        {
            std::string strTmp;
            strTmp.insert(0, startPtr, currPtr - startPtr);
            conditions.push_back(strTmp);
            startPtr = currPtr + 1;
        }

        currPtr++;
    }

    delete[] szFilterCondition;
}

void TestFrame::OnGetRule()
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

void TestFrame::OnGetFilters()
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

    std::string strFilters;
    fs >> strFilters;

    fs.close();

    if (strFilters.empty())
    {
        ::MessageBox(NULL, _T("Filter condition is empty!"), _T(""), NULL);
        return;
    }

    wchar_t woutput[1024];
    swprintf(woutput, 100, L"%hs", strFilters.c_str());

    m_editFilterPath->SetText(woutput);
}

std::wstring TestFrame::GetCurrentPath()
{
    TCHAR szFilePath[MAX_PATH + 1];

    GetModuleFileName(NULL, szFilePath, MAX_PATH);

    (_tcsrchr(szFilePath, _T('\\')))[1] = 0;

    return std::wstring(szFilePath);
}