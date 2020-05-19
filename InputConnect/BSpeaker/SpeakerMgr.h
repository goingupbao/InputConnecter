#pragma once

class CSpeakerMgr:public CWindowWnd
{
public:
    CSpeakerMgr();
    ~CSpeakerMgr();
    class ISpeakerListener
    {
    public: 
        virtual void OnStartSpeakerParagraph(int nIndex, ParagraphInfo pi) = 0;
        virtual void OnFinishChapter(int nIndex) = 0;
    };

    void Init();
    void UnInit();
    void SpeakText(LPCTSTR strText);
    void SpeakParagraph(ParagraphInfo& pi);
    void StartSpeak(int nStartIndex = 0);
    void PauseSpeak();
    void ResumeSpeak();
    void StopSpeak();
    void SetVoiceType(int nType);
    void SetVoiceSpeed(int nSpeed);
    void SetVolume(USHORT nVol);

    void SetChapter(int nIndex);
    void ParserParagraph();

    vector<CString> EnumAllVoice();

    void OnParagraphEnd();
    void OnParagraphStart();
    void SetSpeakerListener(ISpeakerListener *);

    virtual LPCTSTR GetWindowClassName() const;
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual void OnFinalMessage(HWND hWnd);

    bool IsPause();
public :
    Paragraph & GetCurrentParagraph();
private:
    int             m_bSpeaking;
    int             m_nCurrentChapter;
    Paragraph       m_CurrentParagraph;
    ISpVoice *      m_pSpVoice;

    ISpeakerListener*m_pSpeakerListener;

};

