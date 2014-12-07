if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_executable(ukc_cli ukc_cli.cpp)

    target_link_libraries(ukc_cli ukc_core)
endif()
