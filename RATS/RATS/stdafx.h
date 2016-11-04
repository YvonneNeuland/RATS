// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define _WIN32_WINNT 0x0601
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include "targetver.h"
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <iostream>
// TODO: reference additional headers your program requires here
#include "Dependencies/Paramesium/Include/steam_api.h"
#pragma comment(lib, "steam_api.lib")

#include <DirectXMath.h>

#define POS_ORIGIN DirectX::XMFLOAT3(0,0,0)
