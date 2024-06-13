#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include "phnt_windows.h"
#include "phnt.h"
#include <objbase.h>
#include <oleacc.h>
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <ShlObj.h>
#include <shlwapi.h>

#ifdef _DEBUG
#define ILog(data, ...) printf(data, __VA_ARGS__)
#else
#define ILog(data, ...)
#endif

