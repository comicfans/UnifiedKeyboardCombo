#include <windows.h>
#include <signal.h>

#include <iostream>

#include "HookDll.h"


void SignalHandler(int signal)
{
    UninstallHook();
}

int wmain(int argc,wchar_t ** argv){

    signal(SIGABRT,SignalHandler);
    signal(SIGSEGV,SignalHandler);
    signal(SIGINT,SignalHandler);
    signal(SIGTERM,SignalHandler);


    HWND mainWindow=FindWindowEx(NULL,NULL,
            L"VirtualKeyboardCombo",L"VirtualKeyboardCombo");

    if(mainWindow==NULL || mainWindow==INVALID_HANDLE_VALUE){
        //WTF INVALID_HANDLE_VALUE?

        std::cout<<"can not find main window"<<std::endl;
        return EXIT_FAILURE;
    }

    if(!InstallHook(mainWindow)){

        std::cout<<"install hook failed"<<std::endl;
        return EXIT_FAILURE;
    }

        
    std::cout<<"install hook success, sleep forever"<<std::endl;
    Sleep(INFINITE);    
    UninstallHook();
    return EXIT_SUCCESS;
}
