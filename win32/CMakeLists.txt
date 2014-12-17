add_library(ukc_hook 
    SHARED
    HookDll.cpp
    )

target_link_libraries(ukc_hook ukc_core)

if(MSVC)
    set_target_properties(ukc_hook PROPERTIES PREFIX "lib")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set_target_properties(ukc_hook PROPERTIES SUFFIX "64.dll")
else()
    set_target_properties(ukc_hook PROPERTIES SUFFIX "32.dll")
endif()



add_executable(ukc_hook_main HookMain.cpp  
    WatchConfigFileChange.cpp)
set_target_properties(ukc_hook_main 
    PROPERTIES COMPILE_FLAGS "-mwindows")
target_link_libraries(ukc_hook_main ukc_hook ukc_core)


add_executable(ToKeyMap ToKeyMap.cpp)


