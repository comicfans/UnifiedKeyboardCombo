#pragma once

#include <windows.h>

#ifdef __cplusplus
extern "C"{
#endif

typedef struct {
	HANDLE quitEvent;
	HWND mainWnd;
}WatchThreadParam;

DWORD WINAPI watchConfigChangeThread(LPVOID lparam);

#ifdef __cplusplus
}
#endif
