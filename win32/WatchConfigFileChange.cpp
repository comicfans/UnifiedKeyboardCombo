#include "WatchConfigFileChange.h"

#include <memory>
#include <boost/property_tree/json_parser.hpp>

#include <winbase.h>

#include "Profile.hpp"

static bool readFileChange(HANDLE watchHandle){

    //WTF ?
    /* 
     FindFirstChangeNotification handle as input for ReadDirectoryChangesW
    what undocumented is, is that you can also use a FindFirstChangeNotification handle in the call to ReadDirectoryChangesW

    so this works:

    lv_hDirNotify = FindFirstChangeNotification(... lv_hEvent);

    ...

    if (WaitForSingleObject(lv_hEvent, INFINITE) == 0)
    {
    ReadDirectoryChangesW(lv_hDirNotify, ...);

    FindNextChangeNotification(lv_hDirNotify);
    }


    tested on XP-SP3


    .. althrough there is some mixup as Findxxx gives faster/more notifications then Readxxx and therefore the Readxxx will hang after some changes 
    */

    static const auto BUFFER_SIZE=10000;
    std::unique_ptr<DWORD[]> buffer(new DWORD[BUFFER_SIZE]);

    DWORD bufferUsed=0;

    ReadDirectoryChangesW(watchHandle,buffer.get(),BUFFER_SIZE,FALSE,
            FILE_NOTIFY_CHANGE_FILE_NAME|FILE_NOTIFY_CHANGE_LAST_WRITE|FILE_NOTIFY_CHANGE_CREATION,
            &bufferUsed,NULL,NULL);

    int leftSize=bufferUsed;


    while(leftSize>sizeof(FILE_NOTIFY_INFORMATION)){
    
        FILE_NOTIFY_INFORMATION *change=reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer.get()+bufferUsed-leftSize);

        if(wcsncmp(DEFAULT_CONFIG_JSON,change->FileName,change->FileNameLength)==0){
            return true;
        }

        if(change->NextEntryOffset==0){
            return false;
        }

        leftSize-=change->NextEntryOffset;
    }

    return false;

}

DWORD WINAPI watchConfigChangeThread(LPVOID lparam){

	WatchThreadParam *p=(WatchThreadParam*)lparam;

    TCHAR nameBuff[MAX_PATH];

    GetModuleFileName(NULL,nameBuff,MAX_PATH);

    StringType fullName=nameBuff;
     
    fullName=fullName.substr(0,fullName.find_last_of(_T("\\")));

    auto watchHandle=FindFirstChangeNotification(
            fullName.data(),FALSE,
            FILE_NOTIFY_CHANGE_FILE_NAME|FILE_NOTIFY_CHANGE_LAST_WRITE
            );

    if ((watchHandle==INVALID_HANDLE_VALUE) || 
            //WTF?
            (watchHandle == nullptr))    {
        return 1;
        
    }

    HANDLE handles[2]={watchHandle,p->quitEvent};


	while(true){


        auto quit=false;

        auto waitStatus = WaitForMultipleObjects(2,handles,FALSE,INFINITE);

        switch(waitStatus){
            case WAIT_OBJECT_0:{
                if(readFileChange(watchHandle)){
                    PostMessage(p->mainWnd,WM_CONFIG_CHANGE,0,0);
                }
                break;
            }
            case WAIT_OBJECT_0+1:{
                quit=true;
                break;
            }

        }

        if (quit){
            break;
        }
	}

    FindCloseChangeNotification(watchHandle);

	return 1;
}

