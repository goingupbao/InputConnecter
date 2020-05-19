#include "stdafx.h"
#include "BookManager.h"
#include <regex>



CBookManager::CBookManager() :
    m_pCurrentBook(NULL),
    m_pListener(NULL), 
    m_bStop(FALSE)
{
    InitializeCriticalSection(&m_csRead);
}


CBookManager::~CBookManager()
{
    DeleteCriticalSection(&m_csRead);
}


void CBookManager::AddParserdCapters(Chapter * pChapter)
{
    EnterCriticalSection(&m_csRead);
    m_pCurrentBook->_vCapters.push_back(pChapter);
    LeaveCriticalSection(&m_csRead);
}

void CBookManager::ParserCapters()
{
    if (!m_pCurrentBook||m_Title._nCount == 0)
    {
        return;
    }

    ClearInfo();
    wstring strRegex = m_Title._sTitle;
    strRegex.insert(1, L"((?:[1234567890零一二三四五六七八九十百千万])+)");
    //strRegex += L".*";
    wregex re(strRegex);

    TCHAR * pBuff = m_pCurrentBook->_psContext;
    DWORD dwFileSize = m_pCurrentBook->_nLen;
    wstring strText(pBuff,m_pCurrentBook->_nLen);


    //m_vCapters.(1000);
    std::wsregex_iterator begin(strText.begin(), strText.end(), re);
    int i = 0;
    int nlastStart = 0;
    Chapter* cptlast = NULL;
    for (auto iter = begin; iter != std::wsregex_iterator()&& !m_bStop; iter++, i++)
    {
        int nStartPos = iter->position();
       // int nLen = (iternext == std::wsregex_iterator()) ? dwFileSize - nStartPos : iternext->position(i + 1) - nStartPos;
        if (nStartPos >= dwFileSize)
        {
            break;
        }
        Chapter* cpt = new Chapter;
        ZeroMemory(cpt, sizeof(cpt));
        cpt->_nCapID = i;
        cpt->_nStart = nStartPos;
        ZeroMemory(cpt->_sTitle, sizeof(TCHAR)*MAX_PATH);

        int nTitleend = CBookManager::FindStr(m_pCurrentBook->_psContext + nStartPos, L'\n');
        if (nTitleend!=-1)
        {
            memcpy(cpt->_sTitle, m_pCurrentBook->_psContext + nStartPos, min(MAX_PATH, nTitleend * sizeof(TCHAR)));
        }

        while (cptlast!=NULL)
        {
            int nLen = nStartPos -cptlast->_nStart-1;
            if (nLen <= 40) //小章节抛弃
            {
                m_pCurrentBook->_vCapters.pop_back();
                if (m_pCurrentBook->_vCapters.size()>0)
                {
                    cptlast = m_pCurrentBook->_vCapters[m_pCurrentBook->_vCapters.size() - 1];
                }
                else {
                    cptlast = NULL;
                }
                continue;
            }


            cptlast->_nLen = nLen;
         
            cptlast->_psContext = new TCHAR[nLen + 1];

            ZeroMemory(cptlast->_psContext, sizeof(TCHAR)*(nLen + 1));
            memcpy(cptlast->_psContext, pBuff + cptlast->_nStart, sizeof(TCHAR)*(nLen));
            if (m_pListener)
            {
                m_pListener->OnParser(m_pCurrentBook->_vCapters.size(), nStartPos * 100 / m_pCurrentBook->_nLen);
            }
            break;
        }
        cptlast = cpt;
        nlastStart = nStartPos;
        m_pCurrentBook->_vCapters.push_back(cpt);
       
    }
    //std::vector<Chapter *>(m_vCapters).swap(m_vCapters);
    if (cptlast)
    {
        int nLen = dwFileSize - nlastStart;
        cptlast->_nLen = nLen;
        cptlast->_psContext = new TCHAR[nLen + 1];

        ZeroMemory(cptlast->_psContext, sizeof(TCHAR)*(nLen + 1));
        memcpy(cptlast->_psContext, pBuff + cptlast->_nStart, sizeof(TCHAR)*(nLen + 1));
    }
    if (m_pListener)
    {
        m_pListener->OnParserEnd();
    }
}

void CBookManager::CountCaptersTitle(vector<TitleSearch> & titles ,wstring strText)
{

   // wregex e(L".([1234567890一二三四五六七八九十百千万])+.");
    wregex e(L"第((?:[1234567890一二三四五六七八九十百千万])+).");
    

    std::wsregex_iterator begin(strText.begin(), strText.end(), e);
    int i = 0;
    for (auto iter = begin; iter != std::wsregex_iterator(); iter++,i++)
    {
        
        wstring strTitle = iter->str();
        if (strTitle.size() >= 3)
        {
            wstring strKey;
            strKey.append(&strTitle.at(0), 1);
            strKey.append(&strTitle.at(strTitle.size() - 1), 1);

            auto finds = std::find_if(titles.begin(), titles.end(), [strKey](TitleSearch& info) {
                return info._sTitle == strKey;
            }); 
            if (finds == titles.end())
            {
                TitleSearch t;
                t._nCount = 1;
                ZeroMemory(t._sTitle, sizeof(CHAR)*MAX_PATH);
                memcpy(t._sTitle, strKey.c_str(), sizeof(TCHAR)*strKey.size());
                titles.push_back(t);
            }
            else
            {
                finds->_nCount++;
            }
        }
    }
}

void CBookManager::SearchCapterTitle()
{
    USES_CONVERSION;
    if (!m_pCurrentBook)
    {
        return;
    }

    m_Title._nCount = 0;

    
    vector<TitleSearch> vTitles;

    wstring strSearch(m_pCurrentBook->_psContext,min(10240*5,m_pCurrentBook->_nLen/5));

    CountCaptersTitle(vTitles, strSearch);

    if (vTitles.size() >0)
    {
        std::sort(vTitles.begin(), vTitles.end(), [](const TitleSearch& t1, const TitleSearch&t2) {
            return t1._nCount < t2._nCount;
        });

        m_Title = vTitles[vTitles.size() -1];
    }
  
}


Chapter* CBookManager::GetBookCapter(UINT nCaptersIndex)
{
    EnterCriticalSection(&m_csRead);
    if (m_pCurrentBook->_vCapters.size() > nCaptersIndex)
    {
        return m_pCurrentBook->_vCapters[nCaptersIndex];
    }
    LeaveCriticalSection(&m_csRead);
    return NULL;
}
int CBookManager::GetCaptersSize()
{
    return m_pCurrentBook->_vCapters.size();
}

vector<Chapter*> CBookManager::GetBookCapters(UINT nCaptersIndex, UINT nCount)
{
    EnterCriticalSection(&m_csRead);
    vector<Chapter*> vFind;
    if (m_pCurrentBook->_vCapters.size() > nCaptersIndex)
    {
        auto begin = m_pCurrentBook->_vCapters.begin() + nCaptersIndex;
        auto end = (m_pCurrentBook->_vCapters.size() <= nCaptersIndex + nCount) ? m_pCurrentBook->_vCapters.end() : m_pCurrentBook->_vCapters.begin() + nCaptersIndex + nCount;
        vFind.insert(vFind.begin(), begin, end);
    }
    LeaveCriticalSection(&m_csRead);
    return vFind;
}

int CBookManager::OpenFile(LPCTSTR strFilePath)
{
    HANDLE m_hFile = ::CreateFile(strFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, \
        FILE_ATTRIBUTE_NORMAL, NULL);

    if (m_hFile == INVALID_HANDLE_VALUE)
    {
        if (m_pListener)m_pListener->OnError(PB_OPEFNFIELERROR);

        return -1;
    }

    DWORD dwFileSize = 0;

    dwFileSize = ::GetFileSize(m_hFile,NULL);

    if (dwFileSize <= 4)
    {
        if (m_pListener)m_pListener->OnError(PB_FILELENERROR);
        CloseHandle(m_hFile);
        return -1;
    }

    char * pBuff = new char[dwFileSize + 1];

    char * pBuffRead = pBuff;
    DWORD dwRead = 0;
    while (dwRead != dwFileSize)
    {
        DWORD dwThisRead = 0;
        ReadFile(m_hFile, pBuffRead, 1024, &dwThisRead, NULL);
        pBuffRead += dwThisRead;
        dwRead += dwThisRead;
        if (dwThisRead == 0)
        {
            break;
        }
    }

    m_pCurrentBook = new Book;

    m_pCurrentBook->_nBookID =10000+m_mapBooks.size();

    m_mapBooks[m_pCurrentBook->_nBookID] = m_pCurrentBook;

    ParserTxtCode(pBuff);

    int nlen = MultiByteToWideChar(CP_UTF8, 0, pBuff, dwFileSize, NULL, 0);

    if (nlen != 0)
    {
        TCHAR *pBuffT = new TCHAR[nlen + 1];
        ZeroMemory(pBuffT, (nlen + 1) * sizeof(TCHAR));
        MultiByteToWideChar(CP_UTF8, 0, pBuff, dwFileSize, pBuffT, nlen);
        m_pCurrentBook->_psContext = pBuffT;
        m_pCurrentBook->_nLen = nlen;
    }

    if (pBuff)
    {
        delete pBuff;
    }

    CloseHandle(m_hFile);
    if (m_ParserThread.joinable())
    {
        m_bStop = TRUE;
        m_ParserThread.join();
    }

    m_bStop = FALSE;
    m_ParserThread = std::thread([&]()
    {
        InitFileInfo();
    }) ;
 
    return m_pCurrentBook->_nBookID;
}

void CBookManager::InitFileInfo()
{
    SearchCapterTitle();

    ParserCapters();
}

void CBookManager::ParserTxtCode(CHAR* pBuff)
{
    m_pCurrentBook->_nCode = CP_ACP;
    if (pBuff[0] == -1 && pBuff[1] == -2) {
        //m_pCurrentBook->_nCode = CP_UNIC;
    }
    else if (pBuff[0] == -2 && pBuff[1] == -1) {
           // m_pCurrentBook->_nCode = TC_UNICODE;
    }
    else if (pBuff[0] == 0xef && pBuff[1] == 0xbb) {
        m_pCurrentBook->_nCode = TC_UTF_8;
   }
}

void CBookManager::ClearInfo()
{
    EnterCriticalSection(&m_csRead);
    for (int i = 0;i<m_pCurrentBook->_vCapters.size();i++)
    {
        Chapter * cpt = m_pCurrentBook->_vCapters[i];
        if (cpt->_psContext)
        {
            delete[] cpt->_psContext;
        }
        delete cpt;
    }
    m_pCurrentBook->_vCapters.clear();
    m_pCurrentBook->_nCurrentCapters = 0;
    LeaveCriticalSection(&m_csRead);
}
void CBookManager::SetParserListener(IParserBookListener* plistener)
{
    m_pListener = plistener;
}
int CBookManager::FindStr(LPTSTR pstr, TCHAR tc)
{
    LPTSTR strBuff = pstr;
    int nPos = 0;
    while (*strBuff !=L'\0')
    {
        if (*strBuff == tc)
        {
            return nPos;
        }
        strBuff++;
        nPos++;
    }
    return -1;
}
