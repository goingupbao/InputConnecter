#include "stdafx.h"
#include "UIOption.h"

namespace DuiLib
{
    COptionUI::COptionUI() : m_bSelected(false), m_dwSelectedBkColor(0), m_dwSelectedTextColor(0)
    {
    }

    COptionUI::~COptionUI()
    {
        if (!m_sGroupName.IsEmpty() && m_pManager) m_pManager->RemoveOptionGroup(m_sGroupName, this);
    }

    LPCTSTR COptionUI::GetClass() const
    {
        return DUI_CTR_OPTION;
    }

    LPVOID COptionUI::GetInterface(LPCTSTR pstrName)
    {
        if (_tcscmp(pstrName, DUI_CTR_OPTION) == 0) return static_cast<COptionUI*>(this);
        return CButtonUI::GetInterface(pstrName);
    }

    void COptionUI::SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit)
    {
        CControlUI::SetManager(pManager, pParent, bInit);
        if (bInit && !m_sGroupName.IsEmpty()) {
            if (m_pManager) m_pManager->AddOptionGroup(m_sGroupName, this);
        }
    }

    LPCTSTR COptionUI::GetGroup() const
    {
        return m_sGroupName;
    }

    void COptionUI::SetGroup(LPCTSTR pStrGroupName)
    {
        if (pStrGroupName == NULL) {
            if (m_sGroupName.IsEmpty()) return;
            m_sGroupName.Empty();
        }
        else {
            if (m_sGroupName == pStrGroupName) return;
            if (!m_sGroupName.IsEmpty() && m_pManager) m_pManager->RemoveOptionGroup(m_sGroupName, this);
            m_sGroupName = pStrGroupName;
        }

        if (!m_sGroupName.IsEmpty()) {
            if (m_pManager) m_pManager->AddOptionGroup(m_sGroupName, this);
        }
        else {
            if (m_pManager) m_pManager->RemoveOptionGroup(m_sGroupName, this);
        }

        Selected(m_bSelected);
    }

    bool COptionUI::IsSelected() const
    {
        return m_bSelected;
    }

    void COptionUI::Selected(bool bSelected, bool bTriggerEvent)
    {
        if (m_bSelected == bSelected) return;
        m_bSelected = bSelected;
        if (m_bSelected) m_uButtonState |= UISTATE_SELECTED;
        else m_uButtonState &= ~UISTATE_SELECTED;

        if (m_pManager != NULL) {
            if (!m_sGroupName.IsEmpty()) {
                if (m_bSelected) {
                    CDuiPtrArray* aOptionGroup = m_pManager->GetOptionGroup(m_sGroupName);
                    for (int i = 0; i < aOptionGroup->GetSize(); i++) {
                        COptionUI* pControl = static_cast<COptionUI*>(aOptionGroup->GetAt(i));
                        if (pControl != this) {
                            pControl->Selected(false, bTriggerEvent);
                        }
                    }
                    if (bTriggerEvent) m_pManager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED);
                }
            }
            else {
                if (bTriggerEvent) m_pManager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED);
            }
        }

        Invalidate();
    }

    bool COptionUI::Activate()
    {
        if (!CButtonUI::Activate()) return false;
        if (!m_sGroupName.IsEmpty()) Selected(true);
        else Selected(!m_bSelected);

        return true;
    }

    void COptionUI::SetEnabled(bool bEnable)
    {
        CControlUI::SetEnabled(bEnable);
        if (!IsEnabled()) {
            if (m_bSelected) m_uButtonState = UISTATE_SELECTED;
            else m_uButtonState = 0;
        }
    }

    LPCTSTR COptionUI::GetSelectedImage()
    {
        return m_diSelectedNormal.sDrawString;
    }

    void COptionUI::SetSelectedImage(LPCTSTR pStrImage)
    {
        if (m_diSelectedNormal.sDrawString == pStrImage && m_diSelectedNormal.pImageInfo != NULL) return;
        m_diSelectedNormal.Clear();
        m_diSelectedNormal.sDrawString = pStrImage;
        Invalidate();
    }

    LPCTSTR COptionUI::GetSelectedHotImage()
    {
        return m_diSelectedHot.sDrawString;
    }

    void COptionUI::SetSelectedHotImage(LPCTSTR pStrImage)
    {
        if (m_diSelectedHot.sDrawString == pStrImage && m_diSelectedHot.pImageInfo != NULL) return;
        m_diSelectedHot.Clear();
        m_diSelectedHot.sDrawString = pStrImage;
        Invalidate();
    }

    void COptionUI::SetSelectedTextColor(DWORD dwTextColor)
    {
        m_dwSelectedTextColor = dwTextColor;
    }

    DWORD COptionUI::GetSelectedTextColor()
    {
        if (m_dwSelectedTextColor == 0) m_dwSelectedTextColor = m_pManager->GetDefaultFontColor();
        return m_dwSelectedTextColor;
    }

    void COptionUI::SetSelectedBkColor(DWORD dwBkColor)
    {
        m_dwSelectedBkColor = dwBkColor;
    }

    DWORD COptionUI::GetSelectedBkColor()
    {
        return m_dwSelectedBkColor;
    }

    DUI_DEPRECATED DWORD COptionUI::GetSelectBkColor()
    {
        return this->GetSelectedBkColor();
    }

    LPCTSTR COptionUI::GetForeImage()
    {
        return m_diFore.sDrawString;
    }

    void COptionUI::SetForeImage(LPCTSTR pStrImage)
    {
        if (m_diFore.sDrawString == pStrImage && m_diFore.pImageInfo != NULL) return;
        m_diFore.Clear();
        m_diFore.sDrawString = pStrImage;
        Invalidate();
    }

    SIZE COptionUI::EstimateSize(SIZE szAvailable)
    {
        if (m_cxyFixed.cy == 0) return CDuiSize(m_cxyFixed.cx, m_pManager->GetFontInfo(GetFont())->tm.tmHeight + 8);
        return CControlUI::EstimateSize(szAvailable);
    }

    void COptionUI::SetFiveStatusImage(LPCTSTR pStrImage)
    {
        m_diNormal.Clear();
        m_diNormal.sDrawString = pStrImage;
        DrawImage(NULL, m_diNormal);
        if (m_diNormal.pImageInfo) {
            LONG width = m_diNormal.pImageInfo->nX / 4;
            LONG height = m_diNormal.pImageInfo->nY / 2;
            m_diNormal.rcBmpPart = CDuiRect(0, 0, width, height);
            m_diNormal.rcDestOffset= CDuiRect(0, 0, width, height);
            if (m_bFloat && m_cxyFixed.cx == 0 && m_cxyFixed.cy == 0) {
                m_cxyFixed.cx = width;
                m_cxyFixed.cy = height;
            }
        }

        m_diPushed.Clear();
        m_diPushed.sDrawString = pStrImage;
        DrawImage(NULL, m_diPushed);
        if (m_diPushed.pImageInfo) {
            LONG width = m_diPushed.pImageInfo->nX / 4;
            LONG height = m_diPushed.pImageInfo->nY / 2;
            m_diPushed.rcBmpPart = CDuiRect(width, 0, width * 2, height);
            m_diPushed.rcDestOffset = CDuiRect(0, 0, width, height);
        }

        m_diHot.Clear();
        m_diHot.sDrawString = pStrImage;
        DrawImage(NULL, m_diHot);
        if (m_diHot.pImageInfo) {
            LONG width = m_diHot.pImageInfo->nX / 4;
            LONG height = m_diHot.pImageInfo->nY / 2;
            m_diHot.rcBmpPart = CDuiRect(width * 2, 0, width * 3, height);
            m_diHot.rcDestOffset = CDuiRect(0, 0, width, height);
        }

        m_diDisabled.Clear();
        m_diDisabled.sDrawString = pStrImage;
        DrawImage(NULL, m_diDisabled);
        if (m_diDisabled.pImageInfo) {
            LONG width = m_diDisabled.pImageInfo->nX / 4;
            LONG height = m_diDisabled.pImageInfo->nY / 2;
            m_diDisabled.rcBmpPart = CDuiRect(width * 3, 0, width * 4, height);
            m_diDisabled.rcDestOffset = CDuiRect(0, 0, width, height);
        }
        ////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////
        m_diSelectedNormal.Clear();
        m_diSelectedNormal.sDrawString = pStrImage;
        DrawImage(NULL, m_diSelectedNormal);
        if (m_diSelectedNormal.pImageInfo) {
            LONG width = m_diSelectedNormal.pImageInfo->nX / 4;
            LONG height = m_diSelectedNormal.pImageInfo->nY / 2;
            m_diSelectedNormal.rcBmpPart = CDuiRect(0, height, width, 2 * height);
            m_diSelectedNormal.rcDestOffset = CDuiRect(0, 0, width, height);
            if (m_bFloat && m_cxyFixed.cx == 0 && m_cxyFixed.cy == 0) {
                m_cxyFixed.cx = width;
                m_cxyFixed.cy = height;
            }
        }

        m_diSelectedPushed.Clear();
        m_diSelectedPushed.sDrawString = pStrImage;
        DrawImage(NULL, m_diSelectedPushed);
        if (m_diSelectedPushed.pImageInfo) {
            LONG width = m_diSelectedPushed.pImageInfo->nX / 4;
            LONG height = m_diSelectedPushed.pImageInfo->nY / 2;
            m_diSelectedPushed.rcBmpPart = CDuiRect(width, height, width * 2, 2 * height);
            m_diSelectedPushed.rcDestOffset = CDuiRect(0, 0, width, height);
        }

        m_diSelectedHot.Clear();
        m_diSelectedHot.sDrawString = pStrImage;
        DrawImage(NULL, m_diSelectedHot);
        if (m_diSelectedHot.pImageInfo) {
            LONG width = m_diSelectedHot.pImageInfo->nX / 4;
            LONG height = m_diSelectedHot.pImageInfo->nY / 2;
            m_diSelectedHot.rcBmpPart = CDuiRect(width * 2, height, width * 3, 2 * height);
            m_diSelectedHot.rcDestOffset = CDuiRect(0, 0, width, height);
        }

        m_diSelectedDisabled.Clear();
        m_diSelectedDisabled.sDrawString = pStrImage;
        DrawImage(NULL, m_diSelectedDisabled);
        if (m_diSelectedDisabled.pImageInfo) {
            LONG width = m_diSelectedDisabled.pImageInfo->nX / 4;
            LONG height = m_diSelectedDisabled.pImageInfo->nY / 2;
            m_diSelectedDisabled.rcBmpPart = CDuiRect(width * 3, height, width * 4, 2 * height);
            m_diSelectedDisabled.rcDestOffset = CDuiRect(0, 0, width, height);
        }
        Invalidate();
    }

    void COptionUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
    {
        if (_tcscmp(pstrName, _T("group")) == 0) SetGroup(pstrValue);
        else if (_tcscmp(pstrName, _T("selected")) == 0) Selected(_tcscmp(pstrValue, _T("true")) == 0);
        else if (_tcscmp(pstrName, _T("selectedimage")) == 0) SetSelectedImage(pstrValue);
        else if (_tcscmp(pstrName, _T("selectedhotimage")) == 0) SetSelectedHotImage(pstrValue);
        else if (_tcscmp(pstrName, _T("foreimage")) == 0) SetForeImage(pstrValue);
        else if (_tcscmp(pstrName, _T("selectedbkcolor")) == 0) {
            if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
            LPTSTR pstr = NULL;
            DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
            SetSelectedBkColor(clrColor);
        }
        else if (_tcscmp(pstrName, _T("selectedtextcolor")) == 0) {
            if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
            LPTSTR pstr = NULL;
            DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
            SetSelectedTextColor(clrColor);
        }
        else if (_tcscmp(pstrName, _T("statusimage")) == 0) SetFiveStatusImage(pstrValue);

        else CButtonUI::SetAttribute(pstrName, pstrValue);
    }

    void COptionUI::PaintStatusImage(HDC hDC)
    {
        if ((m_uButtonState & UISTATE_SELECTED) != 0) {
            if (IsFocused()) m_uButtonState |= UISTATE_FOCUSED;
            else m_uButtonState &= ~UISTATE_FOCUSED;
            if (!IsEnabled()) m_uButtonState |= UISTATE_DISABLED;
            else m_uButtonState &= ~UISTATE_DISABLED;

            if ((m_uButtonState & UISTATE_DISABLED) != 0) {
                DrawImage(hDC, m_diSelectedDisabled);
            }
            else if ((m_uButtonState & UISTATE_PUSHED) != 0) {
                if (!DrawImage(hDC, m_diSelectedPushed))
                    DrawImage(hDC, m_diSelectedNormal);
            }
            else if ((m_uButtonState & UISTATE_HOT) != 0) {
                if (GetFadeAlphaDelta() > 0) {
                    if (m_uFadeAlpha == 0) {
                        m_diHot.uFade = 255;
                        DrawImage(hDC, m_diSelectedHot);
                    }
                    else {
                        m_diNormal.uFade = m_uFadeAlpha;
                        DrawImage(hDC, m_diSelectedNormal);
                        m_diHot.uFade = 255 - m_uFadeAlpha;
                        DrawImage(hDC, m_diSelectedHot);
                    }
                }
                else {
                    if (!DrawImage(hDC, m_diSelectedHot))
                        DrawImage(hDC, m_diSelectedNormal);
                }
            }
            else if ((m_uButtonState & UISTATE_FOCUSED) != 0) {

            }

            if (GetFadeAlphaDelta() > 0) {
                if (m_uFadeAlpha == 255) {
                    m_diSelectedNormal.uFade = 255;
                    DrawImage(hDC, m_diSelectedNormal);
                }
                else {
                    m_diSelectedHot.uFade = 255 - m_uFadeAlpha;
                    DrawImage(hDC, m_diHot);
                    m_diSelectedNormal.uFade = m_uFadeAlpha;
                    DrawImage(hDC, m_diSelectedNormal);
                }
            }
            else {
                DrawImage(hDC, m_diSelectedNormal);
            }

        }
        else
        {
            CButtonUI::PaintStatusImage(hDC);
        }
    }

	void COptionUI::PaintText(HDC hDC)
	{
		if( (m_uButtonState & UISTATE_SELECTED) != 0 )
		{
			DWORD oldTextColor = m_dwTextColor;
			if( m_dwSelectedTextColor != 0 ) m_dwTextColor = m_dwSelectedTextColor;

			if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
			if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();

			if( m_sText.IsEmpty() ) return;
			int nLinks = 0;
			RECT rc = m_rcItem;
			rc.left += m_rcTextPadding.left;
			rc.right -= m_rcTextPadding.right;
			rc.top += m_rcTextPadding.top;
			rc.bottom -= m_rcTextPadding.bottom;

			if( m_bShowHtml )
				CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, m_sText, IsEnabled()?m_dwTextColor:m_dwDisabledTextColor, \
				NULL, NULL, nLinks, m_iFont, m_uTextStyle);
			else
				CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText, IsEnabled()?m_dwTextColor:m_dwDisabledTextColor, \
				m_iFont, m_uTextStyle);

			m_dwTextColor = oldTextColor;
		}
		else
		{
			UINT uSavedState = m_uButtonState;
			m_uButtonState &= ~UISTATE_PUSHED;
			CButtonUI::PaintText(hDC);
			m_uButtonState = uSavedState;
		}
	}
}