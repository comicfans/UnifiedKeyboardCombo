project (UnifiedKeyboardCombo)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules/")


find_package(Boost REQUIRED)


if(WIN32)
    if(MSVC)
        add_definitions(-DUNICODE)
        add_definitions(-D_UNICODE)
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -municode")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -municode -std=c++11")
    endif()
endif()


if(MINGW)

    add_definitions(-DUNICODE)
    add_definitions(-D_UNICODE)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ")

elseif(WIN32)
    #msvc here
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ")
    include_directories(${CMAKE_SOURCE_DIR}/core/linux)
endif()

include_directories(${Boost_INCLUDE_DIR} core)


add_subdirectory(core)

add_subdirectory(cli)

add_subdirectory(executable)

add_subdirectory(gui)

if(WIN32)
    add_subdirectory(win32)
endif()

enable_testing()
add_subdirectory(test)
