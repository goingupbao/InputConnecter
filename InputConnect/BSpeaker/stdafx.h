// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"
#include "Resource.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
#include <windows.h>

// C 运行时头文件
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
// TODO:  在此处引用程序需要的其他头文件

#include "sapi.h"
#include <sphelper.h>

#include "BSpeaker.h"
#include "SpeakerMgr.h"
#include "BookManager.h"
#include "MainFrame.h"
#include "MainApp.h"
