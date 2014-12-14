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

constexpr int WM_CONFIG_CHANGE=WM_APP+123;

#ifdef __cplusplus
}
#endif
