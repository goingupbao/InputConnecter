#pragma once
class CAutoHeightListTextItem :public CListLabelElementUI
{
public:
    CAutoHeightListTextItem();
    ~CAutoHeightListTextItem();


    virtual void SetPos(RECT rc, bool bNeedInvalidate /* = true */)override;
    virtual void SetText(LPCTSTR pstrText)override;

    void CalcTextRC();
};

