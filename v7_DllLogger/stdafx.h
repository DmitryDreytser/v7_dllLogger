// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
//#include <windows.h>
#include <stdlib.h>
#include "wchar.h"
#include "shlwapi.h"
#include "shellapi.h"


#define IMPORT_1C __declspec(dllimport)
/*
#pragma comment (lib, "libs/basic.lib")
*/
#pragma comment (lib, "libs/bkend.lib")
/*
#pragma comment (lib, "libs/blang.lib")
#pragma comment (lib, "libs/br32.lib")
#pragma comment (lib, "libs/dbeng32.lib")
#pragma comment (lib, "libs/editr.lib")
#pragma comment (lib, "libs/frame.lib")
#pragma comment (lib, "libs/moxel.lib")
#pragma comment (lib, "libs/rgproc.lib")
#pragma comment (lib, "libs/seven.lib")
#pragma comment (lib, "libs/txtedt.lib")
#pragma comment (lib, "libs/type32.lib")
#pragma comment (lib, "libs/userdef.lib")
*/
#pragma comment (lib, "version.lib") 
// TODO: reference additional headers your program requires here
