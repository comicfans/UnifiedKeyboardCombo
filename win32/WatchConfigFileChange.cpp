#include "WatchConfigFileChange.h"

#include <memory>
#include <boost/property_tree/json_parser.hpp>

#include <winbase.h>

#include "Profile.hpp"
#include "Utility.hpp"

static FILETIME lastWriteTime={0,0};


static bool readFileChange(HANDLE watchHandle,const StringType& configFullPath){

            
    ukc_log(UKC_TRACE,_T("file changed"),_T("")); 
                
    //http://forums.codeguru.com/showthread.php?441395-FindFirstChangeNotification-amp-ReadDirectoryChangesW-blocks-execution
    //WTF ? ReadDirectoryChangesW always block until next changes happened ?
    //I ends up with check last_write_time every time

    HANDLE configFileHandle=CreateFile(configFullPath.data(),GENERIC_READ,
            FILE_SHARE_READ|FILE_SHARE_WRITE,nullptr,OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,nullptr);

    if(configFileHandle==INVALID_HANDLE_VALUE){
        ukc_log(UKC_DEBUG,configFullPath,_T("not exist"));
        //open failed
        if(lastWriteTime.dwLowDateTime!=0 || lastWriteTime.dwHighDateTime !=0){
            //deleted or being changing without share ,treate as changed 
            ukc_log(UKC_DEBUG,configFullPath,_T(" deleted"));
            lastWriteTime={0,0};
            return true;
        }
            
        return false;
    }


    //file opened
    FILETIME modifiedTime;
    GetFileTime(configFileHandle,nullptr,nullptr,&modifiedTime);
    CloseHandle(configFileHandle);

    if(CompareFileTime(&lastWriteTime,&modifiedTime)!=0){
        ukc_log(UKC_DEBUG,DEFAULT_CONFIG_JSON,_T(" changed"));
        lastWriteTime=modifiedTime;
        return true;
    }

        
    ukc_log(UKC_DEBUG,DEFAULT_CONFIG_JSON,_T(" same"));

    return false;

}

DWORD WINAPI watchConfigChangeThread(LPVOID lparam){

	WatchThreadParam *p=(WatchThreadParam*)lparam;

    HANDLE configFileHandle=CreateFile(p->configFullPath.data(),GENERIC_READ,
            FILE_SHARE_READ|FILE_SHARE_WRITE,nullptr,OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,nullptr);

    if(configFileHandle!=INVALID_HANDLE_VALUE){
        GetFileTime(configFileHandle,nullptr,nullptr,&lastWriteTime);
        CloseHandle(configFileHandle);
    }

    auto watchHandle=FindFirstChangeNotification(
            p->configDirPath.c_str(),FALSE,
            FILE_NOTIFY_CHANGE_FILE_NAME|FILE_NOTIFY_CHANGE_LAST_WRITE
            );

    if ((watchHandle==INVALID_HANDLE_VALUE) || 
            //WTF?
            (watchHandle == nullptr))    {
        return 0;
        
    }

    HANDLE handles[2]={watchHandle,p->quitEvent};


	while(true){


        auto quit=false;

        auto waitStatus = WaitForMultipleObjects(2,handles,FALSE,INFINITE);

        switch(waitStatus){
            case WAIT_OBJECT_0:{
                if(readFileChange(watchHandle,p->configFullPath)){
                    ukc_log(UKC_INFO,_T("sending config file change info"),_T(""));
                    PostMessage(HWND_BROADCAST,p->ukcConfigChangeMessage,0,0);
                    ukc_log(UKC_INFO,_T("sending config file change info complete"),_T(""));
                }
                FindNextChangeNotification(watchHandle);
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

