#pragma once

#include "resource.h"

#define WM_VOICE_NOTIFY     WM_USER+6677

enum TxtCode
{
    TC_GBK = 0,
    TC_UTF_8 = 1,
    TC_UTF_16 = 2,
    TC_UNICODE = 3
};

enum ParserBookError
{
    PB_OK = 0,
    PB_OPEFNFIELERROR,
    PB_FILELENERROR,
    PE_PARSERCHAPTERERROR
};

struct Chapter
{
    int     _nCapID;
    TCHAR   _sTitle[MAX_PATH];
    TCHAR  *_psContext;
    __int64 _nStart;
    __int64 _nLen;
};
struct Book
{
    int     _nBookID;
    int     _nCode;
    TCHAR   _sFilePath[MAX_PATH];
    TCHAR  *_psContext;
    __int64 _nLen;
    std::vector<Chapter*> _vCapters;

    int     _nCurrentCapters;
    int     _nLastError;
};
struct TitleSearch
{
    TCHAR   _sTitle[MAX_PATH];
    int     _nCount;
};

struct ParagraphInfo
{
    int nStart;
    int nLen;
};

struct Paragraph
{
    int                             _nLen;
    TCHAR                       *   _psContext;
    std::vector<ParagraphInfo>      _vParagraphs;
    int                             _nCurrentParagraphs;
};