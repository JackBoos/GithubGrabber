#pragma once

#include <windows.h>
#include <objbase.h>
#include <UIlib.h>

#include "grabber.h"
#include "commonData.h"

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
    void OnGetFilePath();
    void OnGetToken();
    void OnSendComment();
    void OnCloseOutdated();
    std::wstring GetCurrentPath();
    void FilterData();
    int GetActiveDay(std::string& from);

private:
    CEditUI* m_editRepoUrl;
    CEditUI* m_editUserName;
    CEditUI* m_editToken;
    CEditUI* m_editFilePath;
    CEditUI* m_editFilterDay;
    CRichEditUI* m_editComment;
    CRichEditUI* m_editOutput;
    COptionUI* m_optionFilterActiveDay;
    COptionUI* m_optionFilterCreateDay;
    COptionUI* m_optionIssue;
    COptionUI* m_optionPR;
    CCheckBoxUI* m_boxPortBug;
    CCheckBoxUI* m_boxPortFeature;
    CCheckBoxUI* m_boxQuestion;
    CCheckBoxUI* m_boxNeedsRepro;
    CCheckBoxUI* m_boxNeedMoreInfo;
    CCheckBoxUI* m_boxDocument;
    CCheckBoxUI* m_boxVcpkgBug;
    CCheckBoxUI* m_boxVcpkgFeature;
    IteamDataList m_dataList;
    GithubGrabber::grabber* m_gb;
    int m_iFilterDay;
};