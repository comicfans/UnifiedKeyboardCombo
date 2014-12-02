find_package(GTest)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    include_directories(${EVDEV_INCLUDE_DIR})
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
endif()


if(${GTEST_FOUND})
    include_directories(${GTEST_INCLUDE_DIR})
    foreach(test_filename 
            test_scan 
            test_evdev_matcher
            test_utility
            test_multi_matcher)

        add_executable(${test_filename} ${test_filename}.cpp)

        target_link_libraries(${test_filename} 
            ukc_core ${GTEST_BOTH_LIBRARIES} pthread)

        add_test(${test_filename}_test ${test_filename})
    endforeach()
endif()


add_executable(test_json_write test_json_write.cpp)
target_link_libraries(test_json_write ukc_core)

add_executable(test_write_test_json test_write_test_json.cpp)
target_link_libraries(test_write_test_json ukc_core)

add_executable(test_evdev_uinput test_evdev_uinput.cpp)
target_link_libraries(test_evdev_uinput ${EVDEV_LIBRARY})
