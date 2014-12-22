#include "UinputKeyboard.hpp"

#include "input.h"

#include <libevdev/libevdev-uinput.h>

#include <boost/assert.hpp>

#include <unordered_map>


UinputKeyboard::UinputKeyboard(){

    libevdev *evdev=libevdev_new();

    libevdev_set_name(evdev, VIRTUAL_KEYBOARD_NAME);

    libevdev_enable_event_type(evdev, EV_KEY);
    libevdev_enable_event_type(evdev, EV_SYN);


    //enable all key events

    for(int i=KEY_RESERVED+1;i<KEY_CNT;++i){
        libevdev_enable_event_code(evdev,EV_KEY,i,nullptr);
    }

    //enable mouse rel events
    libevdev_enable_event_type(evdev, EV_REL); 
    libevdev_enable_event_code(evdev, EV_REL,REL_X,nullptr); 
    libevdev_enable_event_code(evdev, EV_REL,REL_Y,nullptr); 
    libevdev_enable_event_code(evdev, EV_REL,REL_WHEEL,nullptr); 

    int rc=libevdev_uinput_create_from_device(evdev,LIBEVDEV_UINPUT_OPEN_MANAGED,
            &m_uinputDev);

    libevdev_free(evdev);

    if(rc<0){
        //TODO error
        BOOST_ASSERT_MSG(false,"create uinput failed");
    }

}

void UinputKeyboard::postKeyEvent(unsigned int keyCode,int value){

    libevdev_uinput_write_event(m_uinputDev,EV_KEY,keyCode,value);

    libevdev_uinput_write_event(m_uinputDev,EV_SYN,SYN_REPORT,0);
}

UinputKeyboard::~UinputKeyboard(){
    libevdev_uinput_destroy(m_uinputDev);
}

#define NAME_KEY(s) {#s,UinputKeyboard::s}

int UinputKeyboard::mouseRelActionNameToCode(const std::string& name){

    static std::unordered_map<std::string,int> MOUSE_REL_ACTION_NAME_MAP_CODE={
        NAME_KEY(MOUSE_X_NEG),
        NAME_KEY(MOUSE_X_POS),
        NAME_KEY(MOUSE_Y_NEG),
        NAME_KEY(MOUSE_Y_POS),
        NAME_KEY(MOUSE_HWHEEL_NEG),
        NAME_KEY(MOUSE_HWHEEL_POS),
        NAME_KEY(MOUSE_WHEEL_NEG),
        NAME_KEY(MOUSE_WHEEL_POS),
        NAME_KEY(MOUSE_DPI_NEG),
        NAME_KEY(MOUSE_DPI_POS)
    }; 

    auto it=MOUSE_REL_ACTION_NAME_MAP_CODE.find(name); 
    if (it==MOUSE_REL_ACTION_NAME_MAP_CODE.end())
    {
        return -1; 
    }
    
    return it->second; 

}

static int MOUSE_REL_ACTION_REL_CODE[UinputKeyboard::MOUSE_REL_TYPE_COUNT]={
    REL_X,REL_X,
    REL_Y,REL_Y,
    REL_HWHEEL,REL_HWHEEL,
    REL_WHEEL,REL_WHEEL
}; 


static constexpr uint16_t MAX_FACTOR=0x8000; 
static constexpr uint16_t MIN_FACTOR=1; 

void UinputKeyboard::postMouseEvent(MouseRelAction mouseRelAction){


    if (mouseRelAction==MOUSE_DPI_NEG){
        if (m_virtualDpiFactor==MIN_FACTOR)
        {
            return; 
        }
        m_virtualDpiFactor>>=1; 
        return; 
    }else if(mouseRelAction==MOUSE_DPI_POS){
        if(m_virtualDpiFactor==MAX_FACTOR){
            return; 
        }
        m_virtualDpiFactor<<=1; 
        return; 
    }

    int value=mouseRelAction%2?1:-1; 

    int ev_code=MOUSE_REL_ACTION_REL_CODE[mouseRelAction-MOUSE_X_NEG]; 


    if (mouseRelAction<=MOUSE_Y_POS){
        value *= m_virtualDpiFactor; 
    }

    libevdev_uinput_write_event(m_uinputDev,EV_REL,ev_code,value); 

    libevdev_uinput_write_event(m_uinputDev,EV_SYN,SYN_REPORT,0);
}

