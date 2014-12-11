#include "KeyMap.hpp"


#ifdef __WIN32 

#elif defined __linux
#include <libevdev/libevdev.h>

unsigned int KeyMap::nameToCode(const StringType& name){

    return libevdev_event_code_from_name_n(EV_KEY,name.c_str(),name.length());
}

StringType KeyMap::codeToName(unsigned int code){

    return libevdev_event_code_get_name(EV_KEY,code);
}

#endif
