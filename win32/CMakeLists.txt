add_library(ukc_hook 
    SHARED
    HookDll.cpp
    )

target_link_libraries(ukc_hook)


add_executable(ukc_hook_main HookMain.cpp  
    WatchConfigFileChange.cpp)
set_target_properties(ukc_hook_main 
    PROPERTIES COMPILE_FLAGS "-mwindows")
target_link_libraries(ukc_hook_main ukc_hook)

add_executable(ToKeyMap ToKeyMap.cpp)

if(MSVC)
    set_target_properties(ukc_hook PROPERTIES PREFIX "lib")
endif()
