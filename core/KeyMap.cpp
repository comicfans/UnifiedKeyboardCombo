#include "KeyMap.hpp"
#include "linux/UinputKeyboard.hpp"
#include <algorithm>

#include <libevdev/libevdev.h>

unsigned int KeyMap::toKeyCode()const{

    if (isMouseRel()){
        return UinputKeyboard::mouseRelActionNameToCode(toKey); 
    }

    return libevdev_event_code_from_name_n(EV_KEY,toKey.c_str(),toKey.length());
}


    
unsigned int KeyMap::fromKeyCode()const{

    return libevdev_event_code_from_name_n(EV_KEY,fromKey.c_str(),fromKey.length());
}

static const char REL_PREFIX[]="MOUSE_"; 


bool KeyMap::isMouseRel()const{

    return (toKey.size()>=6) && 
        std::equal(toKey.begin(),toKey.begin()+6,REL_PREFIX); 
}

