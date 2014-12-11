#include "WatchConfigFileChange.h"

#include <memory>
#include <boost/property_tree/json_parser.hpp>

static void readFileChange(HANDLE watchHandle){

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

    static const int BUFFER_SIZE=10000;
    std::unique_ptr<DWORD[]> buffer(new DWORD[BUFFER_SIZE]);

    DWORD bufferUsed=0;

    ReadDirectoryChanges(watchHandle,buffer.get(),BUFFER_SIZE,FALSE,
            FILE_NOTIFY_CHANGE_FILE_NAME|FILE_NOTIFY_CHANGE_LAST_WRITE|FILE_NOTIFY_CHANGE_CREATION,
            &bufferUsed,NULL,NULL);

    int leftSize=bufferUsed;

    while(leftSize<sizeof(FILE_NOTIFY_INFORMATION)){
    
        FILE_NOTIFY_INFORMATION *res=buffer.get();
        break;//?
    }


}

DWORD WINAPI watchConfigChangeThread(LPVOID lparam){

	WatchThreadParam *p=(WatchThreadParam*)lparam;

    TCHAR fileName[MAX_PATH];

    GetModuleFileName(NULL,fileName,MAX_PATH);

    auto watchHandle=FindFirstChangeNotification(
            &fileName,FALSE,
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

        auto waitStatus = WaitForMultiplyObjects(2,handles,FALSE,INFINITE);

        switch(waitStatus){
            case WAIT_OBJECT_0:{
                readFileChange(watchHandle);
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

