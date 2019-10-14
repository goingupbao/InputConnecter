// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"
#include "Resource.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�: 
#include <windows.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <atlstr.h>
#include <map>
#include <thread>

#define UILIB_STATIC
#include <commctrl.h>
#pragma comment( lib, "comctl32.lib" )

#include "../DuiLib/UIlib.h"
using namespace DuiLib;
// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�

#include "sapi.h"
#include <sphelper.h>

#include "BSpeaker.h"
#include "SpeakerMgr.h"
#include "BookManager.h"
#include "MainFrame.h"
#include "MainApp.h"
