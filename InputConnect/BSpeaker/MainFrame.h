#pragma once


class CMainFrame :public WindowImplBase,public CBookManager::IParserBookListener, CSpeakerMgr::ISpeakerListener
{
public:
    CMainFrame();
    ~CMainFrame();
public: //WindowImplBase
    virtual void OnFinalMessage(HWND hWnd);
    virtual CDuiString GetSkinFolder();
    virtual CDuiString GetSkinFile();
    virtual UILIB_RESOURCETYPE GetResourceType() const;
    virtual LPCTSTR GetWindowClassName(void) const;
    virtual void Notify(TNotifyUI& msg);

    virtual void InitWindow();

    virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnNcActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);
    virtual LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnClose(UINT, WPARAM, LPARAM, BOOL& bHandled);

    virtual LRESULT OnNcUCHDrawCaption(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnNcUCHDrawFrame(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    void OnBtnClickOpenFile();

    void AddCaptertoListUI(int nItems);

    void OnScrolltoBottom();
    
    void OnSelectListItem();
    void OnSelectVoiceItem();
    void OnSelectSpeedItem();

    void InitComboData();
    void LoadLastBook();
    void RegistHotKey();
    void UnRegistHotKey();
    ////////////////////////////////////
    void OnBtnClickStartToRead();
    void OnBtnClickPausetoRead();

    void OnVolChange();
    void OnSpeedChange();
    ////////////////////////////////
    virtual void OnParser(int nCounts, int nPersent);
    virtual void OnError(int nError);
    virtual void OnParserEnd();

    //////////////////////////////////
    virtual void OnStartSpeakerParagraph(ParagraphInfo pi);
    virtual void OnFinishChapter(int nIndex);


private:
   

    CButtonUI   *   m_pBtnOpen;
    CLabelUI    *   m_pLabProgress;
    COptionUI   *   m_pOptShow;
    CButtonUI   *   m_pBtnScrollto;
    CHorizontalLayoutUI *   m_pLyInfo;
    CRichEditUI *   m_pEditFilePath;
    CRichEditUI *   m_pEditCurrentText;
    CListUI     *   m_pListCAP;
    CComboUI    *   m_pComboVoice;
    CButtonUI   *   m_pBtnStart;
    CButtonUI   *   m_pBtnPause;

    CSliderUI   *   m_pSliderSpeed;
    CLabelUI    *   m_pSliderNumSpeed;
    CSliderUI   *   m_pSliderVol;
    CLabelUI    *   m_pSliderNumVol;
    
    BOOL            m_bDataReady;
    BOOL            m_bLoadLastReady;
};

