#pragma once

#include <windows.h>
#include "StringType.hpp"

typedef struct {
	HANDLE quitEvent;
	HWND mainWnd;
    StringType configDirPath;
    StringType configFullPath;
    UINT ukcConfigChangeMessage;
}WatchThreadParam;

DWORD WINAPI watchConfigChangeThread(LPVOID lparam);

