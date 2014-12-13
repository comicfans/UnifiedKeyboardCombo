add_library(ukc_hook 
    SHARED
    HookDll.cpp
    WatchConfigFileChange.cpp
    )

target_link_libraries(ukc_hook ukc_core)


add_executable(ukc_hook_main HookMain.cpp)
set_target_properties(ukc_hook_main 
    PROPERTIES COMPILE_FLAGS "-mwindows")
target_link_libraries(ukc_hook_main ukc_hook)
