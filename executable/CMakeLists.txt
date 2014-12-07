if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_executable(test_daemon test_daemon.cpp)
    target_link_libraries(test_daemon ukc_core)
endif()
