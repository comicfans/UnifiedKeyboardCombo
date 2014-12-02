project (UnifiedKeyboardCombo)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules/")


find_package(Boost COMPONENTS REQUIRED)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -fsanitize=address)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fsanitize=address")

include_directories(${Boost_INCLUDE_DIR} core)


add_subdirectory(core)

add_subdirectory(cli)

add_subdirectory(executable)

add_subdirectory(gui)

enable_testing()
add_subdirectory(test)
