if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_library(Evdev REQUIRED)
    include_directories(${EVDEV_INCLUDE_DIR})
    add_subdirectory(linux)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_subdirectory(win32)
endif()


set(ukc_core_srcs ${ukc_core_srcs}
    MultiMatcher.cpp DeviceMatcher.cpp 
    KeyMap.cpp MainLoop.cpp
    NotMatcher.cpp
    GlobalInit.cpp
    Utility.cpp
    Profile.cpp
    )


add_library(ukc_core 
    STATIC
    ${ukc_core_srcs}
    )


if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message("${EVDEV_LIBRARIES}")
    target_link_libraries(ukc_core ${EVDEV_LIBRARY})
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
endif()


