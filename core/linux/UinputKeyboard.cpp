#include "UinputKeyboard.hpp"

#include "input.h"

#include <libevdev/libevdev-uinput.h>

#include <boost/assert.hpp>


UinputKeyboard::UinputKeyboard(){

    libevdev *evdev=libevdev_new();

    libevdev_set_name(evdev, "UnifiedKeyboardCombo virtual keyboard");

    libevdev_enable_event_type(evdev, EV_KEY);
    libevdev_enable_event_type(evdev, EV_SYN);

    //enable all key events

    for(int i=KEY_RESERVED+1;i<KEY_CNT;++i){
        libevdev_enable_event_code(evdev,EV_KEY,i,nullptr);
    }


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

    libevdev_uinput_write_event(m_uinputDev,EV_SYN,SYN_REPORT,NULL);
}

UinputKeyboard::~UinputKeyboard(){
    libevdev_uinput_destroy(m_uinputDev);
}

