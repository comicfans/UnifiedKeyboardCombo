/*
	This project serves as a simple demonstration for the article "Combining Raw Input and keyboard Hook to selectively block input from multiple keyboards",
	which you should be able to find in this project folder (HookingRawInput.html), or on the CodeProject website (http://www.codeproject.com/).
	The project is based on the idea shown to me by Petr Medek (http://www.hidmacros.eu/), and is published with his permission, huge thanks to Petr!
	The source code is licensed under The Code Project Open License (CPOL), feel free to adapt it.
	Vít Blecha (sethest@gmail.com), 2014
*/

#include <stdio.h>
#include <set>
#include "HookDll.h"

#pragma data_seg (".SHARED")
// HWND of the main executable (managing application)
HWND hwndServer = NULL;


#pragma data_seg ()
#pragma comment (linker, "/section:.SHARED,RWS")

static HINSTANCE instanceHandle=NULL;
static HHOOK hookHandle=NULL;

static DWORD dwTlsIndex;

typedef std::set<HWND> HWNDSet;
struct TlsData{

	HWNDSet hwndRegistered;
};

static TlsData* checkAndCreateTlsData(){

	LPVOID data=TlsGetValue(dwTlsIndex);
	if(data){
		return (TlsData*)data;
	}
	/** WTF  ??
	Note  The local functions have greater overhead and 
	provide fewer features than other memory management 
	functions. New applications should use the heap functions 
	unless documentation states that a local function should be used.
	For more information, see Global and Local Functions.
	*/

	//I guess process default heap has no HEAP_NO_SERIALIZE flag
	//so this can be thread safe
	LPVOID pData=LocalAlloc(LPTR,sizeof(TlsData));

	if(pData==NULL){
		return NULL;
	}

	TlsData * ret=(TlsData*)pData;

	new (&ret->hwndRegistered)HWNDSet;

	TlsSetValue(dwTlsIndex,pData);

	return (TlsData*)pData;

}

static void prepareRawDevice(PRAWINPUTDEVICE array,int size){
	//currently support keyboard only
	RAWINPUTDEVICE &keyboard=array[0];
	
	keyboard.usUsagePage=1;
	keyboard.usUsage=6;
}

static void removeTlsData(){
	LPVOID p=TlsGetValue(dwTlsIndex);
	if(p){

		TlsData *tlsData=(TlsData*)p;

		HWNDSet::iterator it=tlsData->hwndRegistered.begin();

		for(;it!=tlsData->hwndRegistered.end();++it){

			//WTF https://stackoverflow.com/questions/5313743/winapi-unregister-rawinputdevice

			RAWINPUTDEVICE unreg;

			prepareRawDevice(&unreg,1);
			unreg.dwFlags=RIDEV_REMOVE;
			unreg.hwndTarget=NULL;

			RegisterRawInputDevices(&unreg,1,sizeof(RAWINPUTDEVICE));
		}

		tlsData->hwndRegistered.clear();

		TlsSetValue(dwTlsIndex,NULL);
		TlsFree(dwTlsIndex);
	}
}

BOOL APIENTRY DllMain (HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			instanceHandle = hModule;
			hookHandle = NULL;

			  if ((dwTlsIndex = TlsAlloc()) == TLS_OUT_OF_INDEXES) {
                return FALSE; 
			  }

			break;
		case DLL_THREAD_ATTACH:
			{
				break;
			}
		case DLL_THREAD_DETACH:
			{
				removeTlsData();
				break;
			}
		case DLL_PROCESS_DETACH:
			{
				removeTlsData();
				FreeLibrary(instanceHandle);
				break; 
			}
	}
	return TRUE;
}


static LRESULT CALLBACK GetMessageProc (int code, WPARAM wParam, LPARAM lParam)
{
	if (code < 0)
	{ 
		return CallNextHookEx (hookHandle, code, wParam, lParam);
	}

	//WTF MSDN ?
	// This example leads you to believe that you can call 
	//SetTlsValue when DLL_THREAD_ATTACH occurs and it will 
	//be set for the lifetime of the thread but in reality 
	//(see http://msdn.microsoft.com/en-us/library/windows/desktop/ms682583(v=vs.85).aspx) this event is never called for a thread if it existed before the module which contains this code was loaded. The TLS value will need to be lazy initialized in such situations. 

	TlsData* pTlsData=checkAndCreateTlsData();

	if(!pTlsData){
		//allocate failed
		return CallNextHookEx(hookHandle,code,wParam,lParam);
	}
	
	//we may kicked in
	PMSG pMSG=PMSG(lParam);

	
	//check if we have fake register rawinput for this hwnd

	HWND hwnd=pMSG->hwnd;

	HWNDSet::iterator it=pTlsData->hwndRegistered.find(hwnd);

	if(it!=pTlsData->hwndRegistered.end()){
		//already registered, filter message

		if(pMSG->message==WM_INPUT){

			//magic here

			UINT bufferSize;

			// Prepare buffer for the data
			GetRawInputData ((HRAWINPUT)pMSG->lParam, RID_INPUT, NULL, &bufferSize, sizeof (RAWINPUTHEADER));
			LPBYTE dataBuffer = new BYTE[bufferSize];
			// Load data into the buffer
			GetRawInputData((HRAWINPUT)pMSG->lParam, RID_INPUT, dataBuffer, &bufferSize, sizeof (RAWINPUTHEADER));

			RAWINPUT* raw = (RAWINPUT*)dataBuffer;

			// Prepare string buffer for the device name
			GetRawInputDeviceInfo (raw->header.hDevice, RIDI_DEVICENAME, NULL, &bufferSize);
			WCHAR* stringBuffer = new WCHAR[bufferSize];

			// Load the device name into the buffer
			GetRawInputDeviceInfo (raw->header.hDevice, RIDI_DEVICENAME, stringBuffer, &bufferSize);


			// Get the virtual key code of the key and report it
			USHORT virtualKeyCode = raw->data.keyboard.VKey;
			USHORT keyPressed = raw->data.keyboard.Flags & RI_KEY_BREAK ? 0 : 1;
			WCHAR text[128];
			swprintf_s (text, 128, L"Raw Input %s: %X (%d)\n", stringBuffer,virtualKeyCode, keyPressed);
			OutputDebugString (text);

			delete[] stringBuffer;
			delete[] dataBuffer;
			

			DefWindowProc(hwnd,pMSG->message,pMSG->wParam,pMSG->lParam);

			return 0;
		}

		if(pMSG->message==WM_KEYUP || pMSG->message==WM_KEYDOWN ||
			pMSG->message==WM_SYSKEYUP || pMSG->message==WM_SYSKEYDOWN){

				OutputDebugString(L"handle key up down");
				return CallNextHookEx(hookHandle,code,wParam,lParam);
		}


		return CallNextHookEx(hookHandle,code,wParam,lParam);
	}

	//have not registered;
	//register rawinput

	RAWINPUTDEVICE keyboard;

	//WTF magic number
	prepareRawDevice(&keyboard,1);
	keyboard.dwFlags=0;
	keyboard.hwndTarget=hwnd;

	//why last parameter passed ? OS didn't know it?
	RegisterRawInputDevices(&keyboard,1,sizeof(RAWINPUTDEVICE));
	pTlsData->hwndRegistered.insert(it,hwnd);

	return CallNextHookEx(hookHandle,code,wParam,lParam);
}

BOOL InstallHook (HWND hwndParent)
{
	

	// Register keyboard Hook
	hookHandle = SetWindowsHookEx (WH_GETMESSAGE, (HOOKPROC)GetMessageProc, instanceHandle, 0);
	if (hookHandle == NULL)
	{
		return FALSE;
	}
	
	return TRUE;
}

BOOL UninstallHook ()
{
	if (hookHandle == NULL)
	{
		return TRUE;
	}
	// If unhook attempt fails, check whether it is because of invalid handle (in that case continue)
	if (!UnhookWindowsHookEx (hookHandle))
	{
		DWORD error = GetLastError ();
		if (error != ERROR_INVALID_HOOK_HANDLE)
		{
			return FALSE;
		}
	}
	
	hookHandle = NULL;
	return TRUE;
}
