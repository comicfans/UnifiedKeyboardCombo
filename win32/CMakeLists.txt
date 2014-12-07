add_library(ukc_hook 
    SHARED
    HookDll.cpp
    )

target_link_libraries(ukc_hook ukc_core)


add_executable(ukc_hook_main HookMain.cpp)
target_link_libraries(ukc_hook_main ukc_hook)
