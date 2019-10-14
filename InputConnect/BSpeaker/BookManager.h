#pragma once
class CBookManager
{
public:
    CBookManager();
    ~CBookManager();

    class IParserBookListener
    {
    public:
        virtual void OnParser(int nCounts, int nPersent) = 0;
        virtual void OnError(int nError) = 0;
        virtual void OnParserEnd() = 0;
    };

    void ParserCapters();

    void AddParserdCapters(Chapter * pChapter);

    void CountCaptersTitle(std::vector<TitleSearch> & title, wstring strText);

    void SearchCapterTitle();

    vector<Chapter*> GetBookCapters(UINT nCaptersIndex, UINT nCount);
    Chapter* GetBookCapter(UINT nCaptersIndex);

    int OpenFile(LPCTSTR strFilePath);
    void InitFileInfo();
    void ParserTxtCode(CHAR* pBuff);
    void ClearInfo();

    void SetParserListener(IParserBookListener* plistener);

    static int FindStr(LPTSTR pstr, TCHAR tc);
private:

    CString                 m_strCurrentString;

    TitleSearch             m_Title;

    Book    *               m_pCurrentBook;

    std::map<int ,Book*>    m_mapBooks;

    IParserBookListener*    m_pListener;

    CRITICAL_SECTION        m_csRead;

    std::thread             m_ParserThread;

    BOOL                    m_bStop;
};

