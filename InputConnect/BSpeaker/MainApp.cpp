#include "stdafx.h"
#include "MainApp.h"
#include "MainFrame.h"
#include "BookManager.h"
#include "SpeakerMgr.h"
CMainApp theApp;

#define  HS_CONFIG_FILE L"config.ini"

#define UI_WNDSTYLE_SKINDLG (WS_VISIBLE | WS_POPUP |WS_CAPTION| WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME)
CMainApp::CMainApp():m_pMainFrame(NULL),
m_pBookMgr(NULL),
m_pSpeakerMgr(NULL)
{
}


CMainApp::~CMainApp()
{
}

void CMainApp::Init(HINSTANCE hins)
{
    CoInitialize(NULL);
    m_hInstance = hins;

    m_pBookMgr = new CBookManager;
    m_pSpeakerMgr = new CSpeakerMgr;
    m_pSpeakerMgr->Init();
    CPaintManagerUI::SetResourceDefaultType(L"PNG");
    CPaintManagerUI::SetResourceDll(m_hInstance);
    CPaintManagerUI::SetInstance(m_hInstance);
    m_pMainFrame = new CMainFrame;
    m_pMainFrame->Create(NULL, L"BSpeaker", UI_WNDSTYLE_SKINDLG,0);

}

void CMainApp::RunMessageLoop()
{
    CPaintManagerUI::MessageLoop();
}

void CMainApp::UnInit()
{
    if (m_pBookMgr)
    {
        delete m_pBookMgr;
    }
    if (m_pMainFrame)
    {
        delete m_pMainFrame;
    }
    CoUninitialize();
}

CString CMainApp::GetLocalDir()
{
    return CPaintManagerUI::GetInstancePath().GetData();
}

CBookManager* CMainApp::GetBookMgr()
{
    return m_pBookMgr;
}
CSpeakerMgr* CMainApp::GetSpeakerMgr()
{
    return m_pSpeakerMgr;
}
CString CMainApp::GetProfileString(LPCTSTR strAppName, LPCTSTR strKey, LPCTSTR sDef)
{
    CString sConfigPath = CPaintManagerUI::GetInstancePath() + HS_CONFIG_FILE;
    TCHAR buff[MAX_PATH * 10] = { 0 };
    GetPrivateProfileString(strAppName, strKey, sDef, buff, MAX_PATH * 10, sConfigPath);
    return CString(buff);
}

void CMainApp::WriteProfileString(LPCTSTR strAppName, LPCTSTR strKey, LPCTSTR sValue)
{
    CString sConfigPath = CPaintManagerUI::GetInstancePath() + HS_CONFIG_FILE;
    WritePrivateProfileString(strAppName, strKey, sValue, sConfigPath);
}

int CMainApp::GetProfileInt(LPCTSTR strAppName, LPCTSTR strKey, DWORD deft) {
    CString sConfigPath = CPaintManagerUI::GetInstancePath() + HS_CONFIG_FILE;
    return GetPrivateProfileInt(strAppName, strKey, deft, sConfigPath);
}

void CMainApp::WriteProfileInt(LPCTSTR strAppName, LPCTSTR strKey, DWORD dwValue) {
    CString sConfigPath = CPaintManagerUI::GetInstancePath() + HS_CONFIG_FILE;
    CString strValue; strValue.Format(_T("%d"), dwValue);
    WritePrivateProfileString(strAppName, strKey, strValue, sConfigPath);
}

int CMainApp::GetCharCount(LPTSTR strbuff, int nEnd, TCHAR cF)
{
    int nCount = 0;
    int nPos = 0;
    while (*strbuff!=L'\0'&&nPos<=nEnd)
    {
        if (*strbuff == cF)
        {
            nCount++;
        }
        nPos++;
        strbuff++;
    }

    return nCount;
}