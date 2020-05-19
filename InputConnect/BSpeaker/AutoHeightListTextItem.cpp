#include "stdafx.h"
#include "AutoHeightListTextItem.h"


CAutoHeightListTextItem::CAutoHeightListTextItem()
{
}


CAutoHeightListTextItem::~CAutoHeightListTextItem()
{
}

void CAutoHeightListTextItem::SetPos(RECT rc, bool bNeedInvalidate /* = true */)
{
    __super::SetPos(rc, bNeedInvalidate);

    //CalcTextRC();
}
void CAutoHeightListTextItem::SetText(LPCTSTR pstrText)
{
    __super::SetText(pstrText);
    //CalcTextRC();
}

void CAutoHeightListTextItem::CalcTextRC()
{
    RECT rcCalc = {0};
    rcCalc.right -= (m_rcTextPaddingLast.left + m_rcTextPaddingLast.right);
    rcCalc.right += (m_rcItem.right - m_rcItem.left);
    rcCalc.bottom = 999;

    CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcCalc, m_sText, 0, 0, DT_CALCRECT|DT_LEFT|DT_TOP | DT_WORDBREAK);
    
    SetFixedHeight(rcCalc.bottom);
}