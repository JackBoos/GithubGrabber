// GithubGrabber.cpp : Defines the entry point for the application.
//

#include <iostream>
#include "mainFrame.h"
//#define WIN32_LEAN_AND_MEAN	

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
    CPaintManagerUI::SetInstance(hInstance);

    HRESULT Hr = ::CoInitialize(NULL);
    if (FAILED(Hr)) return 0;

    TestFrame* pFrame = new TestFrame();
    if (pFrame == NULL) return 0;
    pFrame->Create(NULL, _T("TestWindow"), UI_WNDSTYLE_FRAME, 0, 0, 0, 0, 0);
    pFrame->CenterWindow();
    ::ShowWindow(*pFrame, SW_SHOW);

    CPaintManagerUI::MessageLoop();

    ::CoUninitialize();

    return 0;
}
