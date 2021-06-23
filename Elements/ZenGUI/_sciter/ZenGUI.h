#pragma once
#include "ZenCommon.h"
#include "ZenCoreCLR.h"
#include "ZenUIBridge.h"
#include <iostream>

#if defined(_WIN32)

#define WINDOW_CLASS_NAME L"sciter-plain-window"
HINSTANCE ghInstance = 0;
#endif

std::wstring s2ws(const std::string &s);
