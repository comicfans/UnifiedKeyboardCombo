find_path(EVDEV_INCLUDE_DIR 
    NAMES libevdev/libevdev.h
    PATHS /usr/include/libevdev-1.0
    )

find_library(EVDEV_LIBRARY 
    NAMES libevdev.so
    HINTS /usr/lib64
    PATHS /usr/lib64/)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBXML2_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Evdev DEFAULT_MSG
    EVDEV_LIBRARY EVDEV_INCLUDE_DIR)



mark_as_advanced(EVDEV_INCLUDE_DIR EVDEV_LIBRARY)
