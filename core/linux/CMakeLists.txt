
find_package(Evdev REQUIRED)

set(linux_srcs 
    ${CMAKE_CURRENT_SOURCE_DIR}/EvdevInputDevice.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/VidPidMatcher.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/HubPortMatcher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/UinputKeyboard.cpp
    )


set(ukc_core_srcs 
    ${linux_srcs}
    PARENT_SCOPE
    )





