#pragma once
class CMainApp
{
public:
    CMainApp();
    ~CMainApp();

    void Init(HINSTANCE hins);
    void RunMessageLoop();
    void UnInit();

    CString GetLocalDir();

    CBookManager* GetBookMgr();
    CSpeakerMgr* GetSpeakerMgr();

    CString					GetProfileString(LPCTSTR strAppName, LPCTSTR strKey, LPCTSTR sDef);

    void					WriteProfileString(LPCTSTR strAppName, LPCTSTR strKey, LPCTSTR sValue);

    int						GetProfileInt(LPCTSTR strAppName, LPCTSTR strKey, DWORD deft);

    void					WriteProfileInt(LPCTSTR strAppName, LPCTSTR strKey, DWORD dwValue);

    int GetCharCount(LPTSTR strbuff ,int nEnd,TCHAR cF);
private:
    HINSTANCE   m_hInstance;

    CMainFrame *m_pMainFrame;
    CBookManager *m_pBookMgr;
    CSpeakerMgr  *m_pSpeakerMgr;
};

extern CMainApp theApp;