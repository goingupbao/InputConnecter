#include "stdafx.h"
#include "MainFrame.h"
#include <commdlg.h>

#define WM_ONPARSER         WM_USER+400
#define WM_ONPARSEREND      WM_USER+401
#define WM_ONPARSERERROR    WM_USER+402
#define WM_SELECTLISTITEM   WM_USER+403
#define WM_MYHOTKEY         WM_USER+404

CMainFrame::CMainFrame()
{
    m_bDataReady = FALSE;
    m_bLoadLastReady = FALSE;
}


CMainFrame::~CMainFrame()
{
}

void CMainFrame::OnFinalMessage(HWND hWnd)
{
    theApp.GetBookMgr()->SetParserListener(NULL);
}

CDuiString CMainFrame::GetSkinFolder()
{
    return L"";
}


UILIB_RESOURCETYPE CMainFrame::GetResourceType() const
{
    return  UILIB_RESOURCETYPE::UILIB_RESOURCE;
}

CDuiString CMainFrame::GetSkinFile()
{
    return L"10000";
}

LPCTSTR CMainFrame::GetWindowClassName(void) const
{
    return L"bsmain";
}

void CMainFrame::InitWindow()
{
    m_pBtnOpen = static_cast<CButtonUI *>( m_PaintManager.FindControl(L"fileopen"));
    m_pLabProgress = static_cast<CLabelUI *>(m_PaintManager.FindControl(L"progress"));
    m_pOptShow = static_cast<COptionUI *>(m_PaintManager.FindControl(L"showinfo"));
    m_pBtnScrollto = static_cast<CButtonUI *>(m_PaintManager.FindControl(L"scrollto"));
    m_pEditFilePath = static_cast<CRichEditUI *>(m_PaintManager.FindControl(L"filepath"));
    m_pEditCurrentText = static_cast<CRichEditUI *>(m_PaintManager.FindControl(L"currenttext"));
    m_pListCAP = static_cast<CListUI *>(m_PaintManager.FindControl(L"textlist"));
    m_pComboVoice = static_cast<CComboUI *>(m_PaintManager.FindControl(L"voicetype"));
    //m_pComboSpeed = static_cast<CComboUI *>(m_PaintManager.FindControl(L"voicespeed"));
    m_pBtnStart = static_cast<CButtonUI *>(m_PaintManager.FindControl(L"play"));
    m_pBtnPause = static_cast<CButtonUI *>(m_PaintManager.FindControl(L"pause"));

    m_pEditCurrentText->HideSelection(false, true);
    m_pLyInfo = static_cast<CHorizontalLayoutUI *>(m_PaintManager.FindControl(L"HInfo"));
    m_pSliderVol = static_cast<CSliderUI *>(m_PaintManager.FindControl(L"voicevol"));
    m_pSliderNumVol = static_cast<CLabelUI *>(m_PaintManager.FindControl(L"volnum"));
    m_pSliderSpeed = static_cast<CSliderUI *>(m_PaintManager.FindControl(L"voicespeed"));
    m_pSliderNumSpeed = static_cast<CLabelUI *>(m_PaintManager.FindControl(L"speednum"));


    CenterWindow();
    theApp.GetBookMgr()->SetParserListener(this);
    theApp.GetSpeakerMgr()->SetSpeakerListener(this);
    InitComboData();

    LoadLastBook();

    RegistHotKey();

    SetIcon(IDI_BSPEAKER);
}

void CMainFrame::Notify(TNotifyUI& msg)
{
    if (msg.sType == DUI_MSGTYPE_CLICK)
    {
        if (msg.pSender == m_pBtnOpen)
        {
            OnBtnClickOpenFile();
        }else if (msg.pSender == m_pBtnStart)
        {
            OnBtnClickStartToRead();
        }else if (msg.pSender == m_pBtnPause)
        {
            OnBtnClickPausetoRead();
        }else if (msg.pSender == m_pBtnScrollto)
        {
            if (m_pListCAP)
            {
                m_pListCAP->EnsureVisible(m_pListCAP->GetCurSel());
            }
        }

    }else if (msg.sType == DUI_MSGTYPE_SELECTCHANGED)
    {
        if (msg.pSender == m_pOptShow)
        {
            if (m_pLyInfo)
            {
                m_pLyInfo->SetVisible(m_pOptShow->IsSelected());
            }

        }
    }
    else if (msg.sType == DUI_MSGTYPE_SCROLL)
    {
        if (msg.pSender == m_pListCAP->GetVerticalScrollBar())
        {
            if (m_pListCAP->GetScrollPos().cy>m_pListCAP->GetScrollRange().cy-30)
            {
                AddCaptertoListUI(50);
            }
        }
    }else if (msg.sType == DUI_MSGTYPE_ITEMSELECT)
    {
        if (msg.pSender == m_pListCAP)
        {
            OnSelectListItem();
        }
        else if (msg.pSender == m_pComboVoice)
        {
            OnSelectVoiceItem();
        }
    }else if (msg.sType == DUI_MSGTYPE_VALUECHANGED)
    {
        if (msg.pSender == m_pSliderSpeed)
        {
            OnSpeedChange();
        }else if (msg.pSender == m_pSliderVol)
        {
            OnVolChange();
        }
    }
    return __super::Notify(msg);
}

LRESULT CMainFrame::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if (uMsg == WM_ONPARSER)
    {
        int nIndex = wParam;
        int nPer = lParam;
        if (m_pLabProgress)
        {
            CString strText; strText.Format(L"%d%%", nPer);
            m_pLabProgress->SetText(strText);
        }
        if (nIndex>=50 || nPer>=20)
        {
            if (!m_bDataReady)
            {
                m_bDataReady = TRUE;
                AddCaptertoListUI(50);
            }
        }
        if (m_pListCAP)
        {
            int nCurrentSel = m_pListCAP->GetCurSel();
            int nIndexOld = theApp.GetProfileInt(L"Main", L"Chapter", -1);
            if (!m_bLoadLastReady&&nCurrentSel == -1&& nIndexOld!=-1 && nCurrentSel!= nIndexOld&&nIndex>nIndexOld)
            {
                m_bLoadLastReady = TRUE;
                AddCaptertoListUI(nIndex);
                PostMessage(WM_SELECTLISTITEM, nIndexOld);
            }
        }
    }
    else if (uMsg == WM_ONPARSERERROR)
    {
        int nError = wParam;
        if (m_pLabProgress)
        {
            m_pLabProgress->SetText(L"解析错误");
        }
    }
    else if (uMsg == WM_ONPARSEREND)
    {
        if (m_pLabProgress)
        {
            m_pLabProgress->SetText(L"解析完成");
        }
    }
    else if (uMsg == WM_SELECTLISTITEM)
    {
        if (m_pListCAP)
        {
            m_pListCAP->SelectItem(wParam);
        }
    }
    else if (uMsg == WM_HOTKEY)
    {
        OnBtnClickPausetoRead();
    }
    return __super::HandleCustomMessage(uMsg, wParam, lParam, bHandled);
}

LRESULT CMainFrame::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
    ::ScreenToClient(*this, &pt);

    RECT rcClient;
    ::GetClientRect(*this, &rcClient);
    bHandled = TRUE;
    RECT rcCaption{ 0, 0, rcClient.right - 120, 50 };
    RECT rcLink{ 325, 25, 400, 40 };
    if (PtInRect(&rcCaption, pt))
    {
        return HTCAPTION;
    }
    return HTCLIENT;
}
LRESULT CMainFrame::OnNcActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
    bHandled = FALSE;
    return 0;
}

LRESULT CMainFrame::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    bHandled = FALSE;
    if (wParam)
    {
        NCCALCSIZE_PARAMS* lpncsp = (NCCALCSIZE_PARAMS*)lParam;

        int nBorder = GetSystemMetrics(SM_CYFRAME);
        int nHeight = GetSystemMetrics(SM_CYMAXIMIZED) - 2 * nBorder;
        int nWidth = GetSystemMetrics(SM_CXMAXIMIZED) - 2 * nBorder;
        int nCaption = GetSystemMetrics(SM_CYCAPTION);

        //lpncsp->rgrc[0].top += 100;


        //lpncsp->rgrc[0].top += 40;
        if (IsZoomed(GetHWND()))
        {
        //    //lpncsp->rgrc[0].top += nBorder;
        //    lpncsp->rgrc[0].left -= 2 * nBorder;
        //    lpncsp->rgrc[0].right += 2 * nBorder;
        //    lpncsp->rgrc[0].bottom += 2 * nBorder;
        }
        //else
        //{
        //    //lpncsp->rgrc[0].left -= 2;
        //    //lpncsp->rgrc[0].right += 2;
        //    //lpncsp->rgrc[0].bottom += 2;
        //}
    }
   
    return 0;// ::OnNcCalcSize(wParam, lParam, bHandled);
}

LRESULT CMainFrame::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    RECT rc;
    GetWindowRect(m_hWnd, &rc);
    RECT rcWindow{ 0, 0, rc.right - rc.left, rc.bottom - rc.top };
    HRGN hRgn = CreateRectRgn(0, 0, rc.right - rc.left, rc.bottom - rc.top);
    SetWindowRgn(GetHWND(), hRgn, TRUE);
    if (hRgn)
    {
        DeleteObject(hRgn);
    }

    return WindowImplBase::OnSize(uMsg, wParam, lParam, bHandled);
}

LRESULT CMainFrame::OnClose(UINT uMsg, WPARAM wp, LPARAM lp, BOOL& bHandled)
{

    UnRegistHotKey();
    LRESULT lr = __super::OnClose(uMsg, wp, lp, bHandled);
    PostQuitMessage(0);
    return lr;
}

LRESULT CMainFrame::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    bHandled = TRUE;

    RECT rc;
    GetClientRect(m_hWnd, &rc);
    RECT rcWnd;//窗口坐标

    GetWindowRect(m_hWnd, &rcWnd);
    ScreenToClient(m_hWnd, (LPPOINT)&rcWnd.left);// 将窗口坐标转换为客户坐标
    ScreenToClient(m_hWnd, (LPPOINT)&rcWnd.right);

    rc.left  = 0; //计算得到窗口内新的客户区域坐标
    rc.top  = 0;
    //rc.right += rc.left;
   // rc.bottom += rc.top;

    HDC hdc = GetWindowDC(m_hWnd);
    //现在rc是一个新的窗口内的客户区坐标
    ExcludeClipRect(hdc, rc.left, rc.top+10, rc.right, rc.bottom); //不刷新客户区域
    //InflateRect(&rc, 30, 30);
    HBRUSH hBr = CreateSolidBrush(RGB(255, 0, 0));
   // FillRect(hdc, &rc, hBr);
    DeleteObject(hBr);
    ReleaseDC(m_hWnd, hdc);

    return 0;
}

LRESULT CMainFrame::OnNcUCHDrawCaption(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = TRUE;
    return 0;
}
LRESULT CMainFrame::OnNcUCHDrawFrame(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

    bHandled = TRUE;
    return 0;
}
void CMainFrame::OnBtnClickOpenFile()
{
    OPENFILENAME opfn;
    WCHAR file_name[MAX_PATH];//file name
    ZeroMemory(&opfn, sizeof(OPENFILENAME));
    opfn.lStructSize = sizeof(OPENFILENAME);//指定这个结构的大小
    opfn.lpstrFilter = L"所有文件\0*.*\0";//指向一对以空字符结束的过滤字符串的一个缓冲。
                                      //缓冲中的最后一个字符串必须以两个 NULL字符结束。
    opfn.nFilterIndex = 1; //指定在文件类型控件中当前选择的过滤器的索引
    opfn.lpstrFile = file_name; //指向包含初始化文件名编辑控件使用的文件名的缓冲
    opfn.lpstrFile[0] = '\0'; //这个缓冲的第一个字符必须是NULL
    opfn.nMaxFile = sizeof(file_name);//指定lpstrFile缓冲的大小，以TCHARs为单位。
                                      //对于ANSI版本，是字节的个数；对于Unicode版本，是字符的个数。
    opfn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST; //OFN_FILEMUSTEXIST 指定用户仅可以在文件名登录字段中
                                                        //输入已存在的文件的名字。
                                                        //如果这个标记被指定的并且用户输入了一个无效的名字，对话框程序显示一个等待消息框。
                                                        //如果这个标记被指定，OFN_PATHMUSTEXIST标记也被使用。
    if (GetOpenFileName(&opfn))
    {
        m_bDataReady = FALSE;
        CString strText = opfn.lpstrFile;
        if (m_pEditFilePath)
        {
            m_pEditFilePath->SetText(strText);
        }
        theApp.WriteProfileString(L"Main", L"file", strText);
        theApp.WriteProfileInt(L"Main", L"Chapter", 0);
        theApp.GetBookMgr()->OpenFile(strText);
    }
}
void CMainFrame::AddCaptertoListUI(int nItems)
{
    if (m_pListCAP)
    {
        int nCurrentAll = m_pListCAP->GetCount();
        std::vector<Chapter *> vcapters =  theApp.GetBookMgr()->GetBookCapters(nCurrentAll, nItems);

        if (vcapters.size()>0)
        {
            for (size_t i = 0 ;i<vcapters.size(); i++)
            {
                Chapter * pchapter = vcapters[i];
                CListLabelElementUI *pItem = new CListLabelElementUI;
                pItem->SetText(pchapter->_sTitle);
                m_pListCAP->Add(pItem);
            }
        }
        m_pListCAP->NeedUpdate();
    }
}

void CMainFrame::OnScrolltoBottom()
{
    AddCaptertoListUI(20);
}

void CMainFrame::OnSelectListItem()
{
    m_pEditCurrentText->SetText(L"");
    int nSelect = m_pListCAP->GetCurSel();
    Chapter * pchapter = theApp.GetBookMgr()->GetBookCapter(nSelect);
    if (pchapter)
    {
        m_pEditCurrentText->SetText(CString(pchapter->_psContext,pchapter->_nLen));
    }
    m_pListCAP->EnsureVisible(nSelect);
    theApp.GetSpeakerMgr()->SetChapter(nSelect);

    theApp.WriteProfileInt(L"Main", L"Chapter", nSelect);
}


void CMainFrame::OnSelectVoiceItem()
{
    if (m_pComboVoice)
    {
        int nIndex = m_pComboVoice->GetCurSel();
        if (nIndex!=-1)
        {
            theApp.GetSpeakerMgr()->SetVoiceType(nIndex);
        }
        theApp.WriteProfileInt(L"Main", L"voice", nIndex);
    }
}
void CMainFrame::LoadLastBook()
{
    CString str = theApp.GetProfileString(L"Main", L"file", L"");
    if (::PathFileExists(str))
    {
        if (m_pEditFilePath)
        {
            m_pEditFilePath->SetText(str);
        }
        theApp.GetBookMgr()->OpenFile(str);
    }
}
void CMainFrame::InitComboData()
{
    if (m_pComboVoice)
    {
        m_pComboVoice->RemoveAll();
        vector<CString> vVoices = theApp.GetSpeakerMgr()->EnumAllVoice();
        for (size_t i = 0 ;i<vVoices.size(); i++)
        {
            CListLabelElementUI *pItem = new CListLabelElementUI;
            pItem->SetText(vVoices[i]);
            m_pComboVoice->Add(pItem);
        }
        size_t nLast = theApp.GetProfileInt(L"Main", L"voice", 0);
        if (nLast<vVoices.size())
        {
            m_pComboVoice->SelectItem(nLast);
        }
    }
    if (m_pSliderSpeed)
    {
        int nLast = theApp.GetProfileInt(L"Main", L"speed", 0);
        m_pSliderSpeed->SetValue(nLast);
        OnSpeedChange();
    }
    if (m_pSliderVol)
    {
        int nLast = theApp.GetProfileInt(L"Main", L"vol", 0);
        m_pSliderVol->SetValue(nLast);
        OnVolChange();   
    }
}


void CMainFrame::RegistHotKey()
{
    ::RegisterHotKey(m_hWnd, WM_MYHOTKEY, NULL, VK_F4);
}
void CMainFrame::UnRegistHotKey()
{

    ::UnregisterHotKey(m_hWnd, WM_MYHOTKEY);
}

////////////////////////////////////
void CMainFrame::OnBtnClickStartToRead()
{
    theApp.GetSpeakerMgr()->StartSpeak();
}
void CMainFrame::OnBtnClickPausetoRead()
{
    int nret = theApp.GetSpeakerMgr()->PauseSpeak();
    if (nret == 0)
    {
        m_pBtnPause->SetText(L"恢复");
    }
    else
    {
        m_pBtnPause->SetText(L"暂停");
    }
}    
void CMainFrame::OnParser(int nCounts, int nPersent)
{
    PostMessage(WM_ONPARSER, nCounts, nPersent);
}
void CMainFrame::OnError(int nError)
{
    PostMessage(WM_ONPARSERERROR, nError, 0);
}
void CMainFrame::OnParserEnd()
{
    PostMessage(WM_ONPARSEREND, 0, 0);
}

void CMainFrame::OnStartSpeakerParagraph(ParagraphInfo pi)
{
    ATLTRACE(L"On Start Speaker Paragraph %d \r\n", pi.nStart);
    if (m_pEditCurrentText)
    {
        Chapter* pChapter = theApp.GetBookMgr()->GetBookCapter(m_pListCAP->GetCurSel());
        int nOffset = 0;
        int nOffsetin = 0;
        if (pChapter)
        {
            nOffset = theApp.GetCharCount(pChapter->_psContext, pi.nStart, L'\n');
            nOffsetin = theApp.GetCharCount(pChapter->_psContext + pi.nStart, pi.nLen, L'\n');
        }

        m_pEditCurrentText->SetSel(pi.nStart - nOffset - nOffsetin + pi.nLen + 1, pi.nStart - nOffset - nOffsetin + pi.nLen + 1);
        m_pEditCurrentText->ScrollCaret();
        m_pEditCurrentText->SetSel(pi.nStart - nOffset, pi.nStart - nOffset - nOffsetin + pi.nLen + 1);
        m_pEditCurrentText->Invalidate();
    }
}
void CMainFrame::OnFinishChapter(int nIndex)
{
    ATLTRACE(L"finish chapter %d", nIndex);
    if (m_pListCAP)
    {
        m_pListCAP->SelectItem(nIndex + 1,false,false);
        OnSelectListItem();
        OnBtnClickStartToRead();
    }
}
void CMainFrame::OnVolChange()
{
    if (m_pSliderVol)
    {
        int nValue = m_pSliderVol->GetValue();
        theApp.WriteProfileInt(L"Main", L"vol", nValue);

        theApp.GetSpeakerMgr()->SetVolume(nValue);
        if (m_pSliderNumVol)
        {
            CString strText;
            strText.Format(L"%d", nValue);
            m_pSliderNumVol->SetText(strText);
        }
    }
}
void CMainFrame::OnSpeedChange()
{
    if (m_pSliderSpeed)
    {
        int nValue = m_pSliderSpeed->GetValue();
        theApp.WriteProfileInt(L"Main", L"speed", nValue);

        theApp.GetSpeakerMgr()->SetVoiceSpeed(nValue);

        if (m_pSliderNumSpeed)
        {
            CString strText;
            strText.Format(L"%d", nValue);
            m_pSliderNumSpeed->SetText(strText);
        }
    }
}