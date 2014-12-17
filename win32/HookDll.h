
#pragma once

#include <windows.h>

#ifdef ukc_hook_EXPORTS
	#define UKC_HOOK_API __declspec(dllexport)
#else
	#define UKC_HOOK_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

UKC_HOOK_API BOOL InstallHook (HWND hwndParent,
        const wchar_t* const configFilePath,UINT ukcConfigChangeMessage);

UKC_HOOK_API BOOL UninstallHook ();

#define UKC_CONFIG_CHANGE_MESSAGE_KEY L"UKC_CONFIG_CHANGE_MESSAGE"

#ifdef __cplusplus
}
#endif
