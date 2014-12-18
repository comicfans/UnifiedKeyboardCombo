add_executable(ukc_cli ukc_cli.cpp)

if(WIN32 AND NOT MSVC)
    set_target_properties(ukc_cli PROPERTIES COMPILE_FLAGS "-DUNICODE -D_UNICODE -municode -mconsole")
endif()

target_link_libraries(ukc_cli ukc_core)
