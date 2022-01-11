#pragma once

#include <windows.h>
#include <objbase.h>
#include <UIlib.h>

using namespace DuiLib;

class TestFrame : public WindowImplBase
{
public:
    TestFrame();
    ~TestFrame();

    virtual CDuiString GetSkinFolder();
    virtual CDuiString GetSkinFile();
    virtual void InitWindow();
    virtual LPCTSTR GetWindowClassName(void) const;
    virtual LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

    virtual void Notify(TNotifyUI& msg);

private:
    void OnGetData();
    void OnGetRule();
    void OnGetFilePath();
    void OnGetToken();
    std::wstring GetCurrentPath();

private:
    CEditUI* m_editBaseUrl;
    CEditUI* m_editRepoUrl;
    CEditUI* m_editSearchUrl;
    CEditUI* m_editUserName;
    CEditUI* m_editToken;
    CEditUI* m_editFilePath;
    COptionUI* m_optionIssue;
    COptionUI* m_optionPR;
};