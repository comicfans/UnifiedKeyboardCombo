
#pragma once

#include <Windows.h>

#ifdef ukc_hook_EXPORTS
	#define UKC_HOOK_API __declspec(dllexport)
#else
	#define UKC_HOOK_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

UKC_HOOK_API BOOL InstallHook (HWND hwndParent);

UKC_HOOK_API BOOL UninstallHook ();

#ifdef __cplusplus
}
#endif
