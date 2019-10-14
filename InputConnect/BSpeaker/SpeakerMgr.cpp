#include "stdafx.h"
#include "SpeakerMgr.h"
#include <regex>

CSpeakerMgr::CSpeakerMgr():
    m_bSpeaking(FALSE),
m_nCurrentChapter(-1),
m_pSpVoice(NULL),
m_pSpeakerListener(NULL)
{
}


CSpeakerMgr::~CSpeakerMgr()
{
}


void CSpeakerMgr::Init()
{
    if (S_OK !=CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_INPROC_SERVER, IID_ISpVoice, (void**)&m_pSpVoice))
    {
        m_pSpVoice = NULL;
    }
    ULONGLONG ullMyEvents = SPFEI(SPEI_START_INPUT_STREAM) | SPFEI(SPEI_END_INPUT_STREAM);
    m_pSpVoice->SetInterest(ullMyEvents, ullMyEvents);

    Create(HWND_MESSAGE, NULL, 0, 0);

    if (GetHWND())
    {
        LRESULT lr = m_pSpVoice->SetNotifyWindowMessage(GetHWND(), WM_VOICE_NOTIFY, 0, 0);
        int i = 0;
    }
}

void CSpeakerMgr::UnInit()
{
    m_pSpVoice->Release();
    m_pSpVoice = NULL;
    Close();
}

LPCTSTR CSpeakerMgr::GetWindowClassName() const
{
    return L"voicemessage";
}
LRESULT CSpeakerMgr::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_VOICE_NOTIFY)
    {
        SPEVENT eventItem;
        memset(&eventItem, 0, sizeof(SPEVENT));
        while (m_pSpVoice&&m_pSpVoice->GetEvents(1, &eventItem, NULL) == S_OK)
        {
            switch (eventItem.eEventId)
            {
            case SPEI_START_INPUT_STREAM:
                OnParagraphStart();
                break;
            case SPEI_END_INPUT_STREAM:
                OnParagraphEnd();
                break;
            default:
                break;
            }

            SpClearEvent(&eventItem);
        }
        return S_OK;
    }
    return ::DefWindowProc(m_hWnd,uMsg, wParam, lParam);
}


void CSpeakerMgr::OnParagraphEnd()
{
    if (m_pSpeakerListener)
    {
        m_CurrentParagraph._nCurrentParagraphs++;
        if (m_CurrentParagraph._nCurrentParagraphs >= m_CurrentParagraph._vParagraphs.size())
        {
            m_pSpeakerListener->OnFinishChapter(m_nCurrentChapter);
        }
        else
        {
            SpeakParagraph(m_CurrentParagraph._vParagraphs[m_CurrentParagraph._nCurrentParagraphs]);
        }
    }
}

void CSpeakerMgr::OnParagraphStart()
{
    if (m_pSpeakerListener)
    {
        if (m_CurrentParagraph._nCurrentParagraphs<m_CurrentParagraph._vParagraphs.size())
        {
            m_pSpeakerListener->OnStartSpeakerParagraph(m_CurrentParagraph._vParagraphs[m_CurrentParagraph._nCurrentParagraphs]);
        }
    }
}


void CSpeakerMgr::SetSpeakerListener(ISpeakerListener * ptr)
{
    m_pSpeakerListener = ptr;
}

void CSpeakerMgr::OnFinalMessage(HWND hWnd)
{

}

vector<CString> CSpeakerMgr::EnumAllVoice()
{
    vector<CString> vVoices;
    IEnumSpObjectTokens * pEnum = NULL;
    if (SUCCEEDED(SpEnumTokens(SPCAT_VOICES, NULL, NULL, &pEnum)))
    {
        //得到所有语音Token的个数
        ULONG ulTokensNumber = 0;
        pEnum->GetCount(&ulTokensNumber);

        //检测该机器是否安装有语音包
        if (ulTokensNumber == 0)
        {
            return vVoices;
        }

        //将语音包的名字加入组合框控件
        ISpObjectToken *pToken = NULL;
        CString strVoicePackageName = _T("");
        CString strTokenPrefixText = _T("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Speech\\Voices\\Tokens\\");
        for (ULONG i = 0; i < ulTokensNumber; i++)
        {
            pEnum->Item(i, &pToken);
            WCHAR* pChar;
            pToken->GetId(&pChar);

            HKEY hKey;
            CString StrKey = pChar;
            StrKey.Replace(L"HKEY_LOCAL_MACHINE\\", L"");
            StrKey += L"\\Attributes";
            if (ERROR_SUCCESS == ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, StrKey, NULL, KEY_QUERY_VALUE, &hKey))
            {
                DWORD dwSize = 100;
                DWORD dwType = REG_SZ;
                LPCTSTR KeyValue = _T("Name");
                TCHAR Name[100] = { 0 };
                ::RegQueryValueEx(hKey, KeyValue, 0, &dwType, (BYTE *)Name, &dwSize);
                ::RegCloseKey(hKey);
                vVoices.push_back(Name);
            }
        }
    }
    return vVoices;
}

void CSpeakerMgr::SpeakText(LPCTSTR strText)
{
    if (m_pSpVoice)
    {
        ULONG uID = 0;
        m_pSpVoice->Speak(strText, SPF_ASYNC| SPF_PURGEBEFORESPEAK, &uID);
    }
}

void CSpeakerMgr::SpeakParagraph(ParagraphInfo &pi)
{
    if (m_pSpVoice)
    {
        ULONG uID = 0;
        TCHAR *pBuff = new TCHAR[pi.nLen + 1];
        ZeroMemory(pBuff, sizeof(TCHAR)*(pi.nLen + 1));
        memcpy(pBuff, m_CurrentParagraph._psContext + pi.nStart, sizeof(TCHAR)*(pi.nLen));
        SpeakText(pBuff);
        delete[]pBuff;
    }
}

void CSpeakerMgr::StartSpeak()
{
    if (m_pSpVoice)
    {
        m_CurrentParagraph._nCurrentParagraphs = 0;
        if (m_CurrentParagraph._vParagraphs.size()>0)
        {
            SpeakParagraph(m_CurrentParagraph._vParagraphs[m_CurrentParagraph._nCurrentParagraphs]);
        }
    }
}

int CSpeakerMgr::PauseSpeak()
{
    if (m_pSpVoice)
    {
        SPVOICESTATUS Status;
        m_pSpVoice->GetStatus(&Status, NULL);
        if (Status.dwRunningState == SPRS_IS_SPEAKING)
        {
            m_pSpVoice->Pause();
            return 0;
        }
        else
        {
            m_pSpVoice->Resume();
            return 1;
        }
    }
}

void CSpeakerMgr::StopSpeak()
{
    if (m_pSpVoice)
    {
        m_pSpVoice->Speak(NULL, SPF_PURGEBEFORESPEAK, NULL);
    }
}

void CSpeakerMgr::SetChapter(int nIndex)
{
    m_nCurrentChapter = nIndex;
    ParserParagraph();
}

void CSpeakerMgr::ParserParagraph()
{
    if (m_CurrentParagraph._psContext)
    {
        m_CurrentParagraph._vParagraphs.clear();
        delete []m_CurrentParagraph._psContext;
        m_CurrentParagraph._psContext = NULL;
    }
    m_CurrentParagraph._nCurrentParagraphs = 0;
    Chapter *pCap = theApp.GetBookMgr()->GetBookCapter(m_nCurrentChapter);
    if (!pCap)return;
    
    m_CurrentParagraph._psContext = new TCHAR[pCap->_nLen + 1];
    ZeroMemory(m_CurrentParagraph._psContext, sizeof(TCHAR)*pCap->_nLen + 1);
    m_CurrentParagraph._nLen = pCap->_nLen;
    memcpy(m_CurrentParagraph._psContext, pCap->_psContext, sizeof(TCHAR)*pCap->_nLen);

    wstring strRegex = L"\n";

    wregex re(strRegex);

    wstring strText(m_CurrentParagraph._psContext, m_CurrentParagraph._nLen);


    //m_vCapters.(1000);
    std::wsregex_iterator begin(strText.begin(), strText.end(), re);
    int nlastStart = 0;
    for (auto iter = begin; iter != std::wsregex_iterator(); iter++)
    {
        int nPos = iter->position();
        int nLen = nPos - nlastStart;
        if (nLen<20)
        {
            continue;
        }
        LPTSTR pstrBuff = m_CurrentParagraph._psContext;
        while(*(pstrBuff + nlastStart) == L'\n'||
            *(pstrBuff + nlastStart) == L'\n'||
            *(pstrBuff + nlastStart) == L' '||
            *(pstrBuff + nlastStart) == L'\t'||
            *(pstrBuff + nlastStart) == 0xA1A1||
            * (pstrBuff + nlastStart) == 0x3000)
        {
            nlastStart++;
        }
        nLen = nPos - nlastStart;
        while (*(pstrBuff + nlastStart+nLen) == L'\n' ||
            *(pstrBuff + nlastStart + nLen) == L'\n' ||
            *(pstrBuff + nlastStart + nLen) == L' ' ||
            *(pstrBuff + nlastStart + nLen) == L'\t' ||
            *(pstrBuff + nlastStart + nLen) == 0xA1A1 ||
            *(pstrBuff + nlastStart + nLen) == 0x3000)
        {
            nLen--;
        }

        ParagraphInfo pi;
        pi.nLen = nLen;
        pi.nStart = nlastStart;
        m_CurrentParagraph._vParagraphs.push_back(pi);
        nlastStart = nPos;
    }
}

void CSpeakerMgr::SetVoiceType(int nType)
{
    if (m_pSpVoice)
    {
        vector<CString> vVoices;
        IEnumSpObjectTokens * pEnum = NULL;
        if (SUCCEEDED(SpEnumTokens(SPCAT_VOICES, NULL, NULL, &pEnum)))
        {
            //得到所有语音Token的个数
            ULONG ulTokensNumber = 0;
            pEnum->GetCount(&ulTokensNumber);

            //检测该机器是否安装有语音包
            if (ulTokensNumber == 0)
            {
                return ;
            }

            //将语音包的名字加入组合框控件
            ISpObjectToken *pToken = NULL;

            if (nType<ulTokensNumber)
            {
                pEnum->Item(nType, &pToken);
                m_pSpVoice->SetVoice(pToken);
            }
        }
    }
}
void CSpeakerMgr::SetVoiceSpeed(int nSpeed)
{
    if (m_pSpVoice)
    {
        m_pSpVoice->SetRate(nSpeed);
    }
}
void CSpeakerMgr::SetVolume(USHORT nVol)
{
    if (m_pSpVoice)
    {
        m_pSpVoice->SetVolume(nVol);
    }
}