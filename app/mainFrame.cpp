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
    m_optionIssue = (COptionUI*)m_PaintManager.FindControl(_T("SET_ISSUE"));
    m_optionPR = (COptionUI*)m_PaintManager.FindControl(_T("SET_PR"));

    m_editBaseUrl->SetText(_T("https://api.github.com/search/"));
    m_editRepoUrl->SetText(_T("microsoft/vcpkg"));
    m_editUserName->SetText(_T("JackBoosY"));

    std::wstring filepath = GetCurrentPath() + _T("result.xlsx");
    m_editFilePath->SetText(filepath.c_str());

    m_optionIssue->Activate();
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
    }
}

void TestFrame::OnGetData()
{
    wchar_t* baseUrl = (wchar_t*)m_editBaseUrl->GetText().GetData();
    char szBaseUrl[256];
    sprintf(szBaseUrl, "%ws", baseUrl);

    wchar_t* repoName = (wchar_t*)m_editRepoUrl->GetText().GetData();
    char szRepoName[256];
    sprintf(szRepoName, "%ws", repoName);

    wchar_t* userName = (wchar_t*)m_editUserName->GetText().GetData();
    char szUserName[256];
    sprintf(szUserName, "%ws", userName);

    wchar_t* token = (wchar_t*)m_editToken->GetText().GetData();
    char szToken[256];
    sprintf(szToken, "%ws", token);

    wchar_t* filePath = (wchar_t*)m_editFilePath->GetText().GetData();
    char szFilePath[256];
    sprintf(szFilePath, "%ws", _T("result.xlsx"));// filePath);

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

    cdtList.push_back("state:open");
    cdtList.push_back("label:\"category:port-bug\"");
    cdtList.push_back("sort:created");
    if (!gb.GetData(SearchCondition{ search_type, search_type_message }, cdtList, szFilePath, 50))
    {
        ::MessageBox(NULL, _T("Failed to get data"), _T(""), NULL);
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

std::wstring TestFrame::GetCurrentPath()
{
    TCHAR szFilePath[MAX_PATH + 1];

    GetModuleFileName(NULL, szFilePath, MAX_PATH);

    (_tcsrchr(szFilePath, _T('\\')))[1] = 0;

    return std::wstring(szFilePath);
}