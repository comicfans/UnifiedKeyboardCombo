find_package(Evdev REQUIRED)

include_directories(${EVDEV_INCLUDE_DIR})

set(linux_srcs 
    ${CMAKE_CURRENT_SOURCE_DIR}/EvdevInputDevice.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/UinputKeyboard.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/EvdevMatcher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MainLoop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/KeyMap.cpp
    )


set(ukc_core_srcs 
    ${linux_srcs}
    PARENT_SCOPE
    )





