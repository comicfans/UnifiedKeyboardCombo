#include "KeyMap.hpp"

#include "input.h"

#include "libevdev-strip.h"

unsigned int KeyMap::nameToCode(const std::string& name){

    return libevdev_event_code_from_name_n(EV_KEY,name.c_str(),name.length());
}

std::string KeyMap::codeToName(unsigned int code){

    return libevdev_event_code_get_name(EV_KEY,code);
}

