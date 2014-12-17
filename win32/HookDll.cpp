/*
	This project serves as a simple demonstration for the article "Combining Raw Input and keyboard Hook to selectively block input from multiple keyboards",
	which you should be able to find in this project folder (HookingRawInput.html), or on the CodeProject website (http://www.codeproject.com/).
	The project is based on the idea shown to me by Petr Medek (http://www.hidmacros.eu/), and is published with his permission, huge thanks to Petr!
	The source code is licensed under The Code Project Open License (CPOL), feel free to adapt it.
	Vít Blecha (sethest@gmail.com), 2014
*/

#include <stdio.h>
#include <string>
#include <set>
#include <fstream>
#include <memory>
#include "HookDll.h"
#include "Profile.hpp"
#include "DeviceMatcher.hpp"
#include "win32/RawInputDevice.hpp"
#include <boost/property_tree/json_parser.hpp>
#include <boost/ptr_container/ptr_unordered_map.hpp>

#pragma data_seg (".SHARED")
// HWND of the main executable (managing application)
HWND hwndServer = NULL;

wchar_t configFilePath[MAX_PATH+1]= L"";

UINT ukcConfigChangeMessage=0;

#pragma data_seg ()
#pragma comment (linker, "/section:.SHARED,RWS")

static HINSTANCE instanceHandle=NULL;
static HHOOK hookHandle=NULL;

static DWORD dwTlsIndex;

using std::vector;

typedef boost::ptr_unordered_map<HANDLE,RawInputDevice> DeviceMapType;
typedef std::set<HWND> HWNDSetType;
struct TlsData{

    HWNDSetType registeredHwnds;
	DeviceMapType registeredDevices;
    vector<Profile> profiles;
    HANDLE previousDevice;
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

	new (&ret->registeredHwnds)HWNDSetType;
	new (&ret->registeredDevices)DeviceMapType;
	new (&ret->profiles)vector<Profile>;
    ret->previousDevice=INVALID_HANDLE_VALUE;

	TlsSetValue(dwTlsIndex,pData);

	return (TlsData*)pData;

}

static void prepareRawDevice(PRAWINPUTDEVICE array,int size){
	//currently support keyboard only
	RAWINPUTDEVICE &keyboard=array[0];
	
	keyboard.usUsagePage=1;
	keyboard.usUsage=6;
}



static void reloadConfig(){
    TreeType readTree;

    try{

        //TODO file load
        std::wifstream file;
    
        boost::property_tree::read_json(file,readTree);

        auto profiles=Profile::readList(readTree);

        if(auto p = checkAndCreateTlsData()){

            p->profiles=profiles;
        }

    }catch(boost::property_tree::json_parser_error &t){

        return;
    }
        
}

static void configureAddIfMatch(std::unique_ptr<RawInputDevice> device){

    auto p=checkAndCreateTlsData();
    if(!p){return;}

    for(auto & profile: p->profiles){
        if(profile.matcher().matchDevice(*device)){

            device->configure(profile.keyMaps(),
                    profile.disableNonKeyEvent(),
                    profile.disableUnmappedKey());

            auto handle=device->handle();
            p->registeredDevices.insert(handle,device.release());

        }
    }
}

static void configureAll(){

    auto p=checkAndCreateTlsData();
    if(!p){return;}

    auto deviceList=InputDevice::scanDevices();

    p->registeredDevices.clear();

    for(auto &inputDevice: deviceList){
        
        configureAddIfMatch(std::move(inputDevice));
    }

}

static void removeTlsData(){
	LPVOID p=TlsGetValue(dwTlsIndex);
	if(!p){
        return;
	}


    //WTF https://stackoverflow.com/questions/5313743/winapi-unregister-rawinputdevice

    RAWINPUTDEVICE unreg;

    prepareRawDevice(&unreg,1);
    unreg.dwFlags=RIDEV_REMOVE;
    unreg.hwndTarget=NULL;

    RegisterRawInputDevices(&unreg,1,sizeof(RAWINPUTDEVICE));


    TlsData *tlsData=(TlsData*)p;
    tlsData->registeredDevices.~ptr_unordered_map<HANDLE,RawInputDevice>();
    tlsData->profiles.~vector<Profile>();
    tlsData->registeredHwnds.~set<HWND>();

    TlsSetValue(dwTlsIndex,NULL);
    TlsFree(dwTlsIndex);
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

#ifdef _WIN64
#define GetMessageProc GetMessageProc64
#else
#define GetMessageProc GetMessageProc32
#endif

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

	auto it=pTlsData->registeredHwnds.find(hwnd);

	if(it!=pTlsData->registeredHwnds.end()){
		//already registered, filter message

        if(pMSG->message==WM_INPUT){
            //magic here

            UINT bufferSize;

            // Prepare buffer for the data
            GetRawInputData ((HRAWINPUT)lParam, RID_INPUT, NULL, &bufferSize, sizeof (RAWINPUTHEADER));
			std::unique_ptr<BYTE[]> dataBuffer(new BYTE[bufferSize]);
            // Load data into the buffer
            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, dataBuffer.get(), &bufferSize, sizeof (RAWINPUTHEADER));

            RAWINPUT* raw = (RAWINPUT*)dataBuffer.get();


            auto it=pTlsData->registeredDevices.find(raw->header.hDevice);

            if(it==pTlsData->registeredDevices.end()){
                
                // not registered device ?
            
    
                return DefWindowProc(hwnd,pMSG->message,pMSG->wParam,pMSG->lParam);
            }


            it->second->nextRaw(hwnd,raw,lParam,wParam);
            pTlsData->previousDevice=raw->header.hDevice;
                
            //ignore next hook (we assume this app didn't hook WM_INPUT)
            return DefWindowProc(hwnd,pMSG->message,pMSG->wParam,pMSG->lParam);
        }

		if(pMSG->message==WM_KEYUP || pMSG->message==WM_KEYDOWN ||
			pMSG->message==WM_SYSKEYUP || pMSG->message==WM_SYSKEYDOWN){


                if(pTlsData->previousDevice==INVALID_HANDLE_VALUE){
                    //ignored
                    return CallNextHookEx(hookHandle,code,wParam,lParam);
                }

                auto it=pTlsData->registeredDevices.find(pTlsData->previousDevice);
                if(it==pTlsData->registeredDevices.end()){
                    //error happened
				
                    OutputDebugString(L"key event without previous rawinput");
                    return CallNextHookEx(hookHandle,code,wParam,lParam);
                }

                it->second->nextKey(hookHandle,code,wParam,lParam);
                return 1;
		}

        if(pMSG->message==ukcConfigChangeMessage){
				
			OutputDebugString(L"custom message");

            //reload config
            reloadConfig();
            configureAll();
            //this is custom ,should be dropped
            return 1;
        }


		return CallNextHookEx(hookHandle,code,wParam,lParam);
	}

	//have not registered;
	//register rawinput

    pTlsData->registeredHwnds.insert(hwnd);

    reloadConfig();

    if(pTlsData->profiles.empty()){
        return CallNextHookEx(hookHandle,code,wParam,lParam);
    }


    configureAll();

    if(pTlsData->registeredDevices.empty()){
        return CallNextHookEx(hookHandle,code,wParam,lParam);
    }


	RAWINPUTDEVICE keyboard;

	//WTF magic number
	prepareRawDevice(&keyboard,1);
	keyboard.dwFlags=0;
	keyboard.hwndTarget=hwnd;

	//why last parameter passed ? OS didn't know it?
	RegisterRawInputDevices(&keyboard,1,sizeof(RAWINPUTDEVICE));

	return CallNextHookEx(hookHandle,code,wParam,lParam);
}

BOOL InstallHook (HWND hwndParent,const wchar_t* const configFilePathSet,UINT ukcConfigChangeMessageSet)
{
	
    if(hwndServer!=NULL){
        return FALSE;
    }
    hwndServer=hwndParent;


	// Register keyboard Hook
	hookHandle = SetWindowsHookEx (WH_GETMESSAGE, (HOOKPROC)GetMessageProc, instanceHandle, 0);
	if (hookHandle == NULL)
	{
		return FALSE;
	}
	
    wcsncpy(configFilePath,configFilePathSet,MAX_PATH);
    ukcConfigChangeMessage=ukcConfigChangeMessageSet;
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
