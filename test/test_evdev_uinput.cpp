#include <libevdev/libevdev-uinput.h>
#include <unistd.h>

#include "input.h"

int main(int argc, char *argv[])
{
     
    libevdev_uinput *m_uinputDev=nullptr;

    libevdev *evdev=libevdev_new();

    libevdev_set_name(evdev, "UnifiedKeyboardCombo virtual keyboard");

    libevdev_set_id_vendor(evdev,120);
    libevdev_set_id_product(evdev,240);
    libevdev_set_id_bustype(evdev,BUS_USB);

    libevdev_enable_event_type(evdev, EV_KEY);

    //enable all key events

        
    libevdev_enable_event_code(evdev,EV_KEY,KEY_W,nullptr);

    int rc=libevdev_uinput_create_from_device(evdev,LIBEVDEV_UINPUT_OPEN_MANAGED,
            &m_uinputDev);

    //libevdev_free(evdev);

    if(rc<0){
        return -1;
    }

   

    libevdev_uinput_write_event(m_uinputDev,EV_KEY,KEY_W,1);
    libevdev_uinput_write_event(m_uinputDev,EV_SYN,SYN_REPORT,NULL);

    libevdev_uinput_write_event(m_uinputDev,EV_KEY,KEY_W,0);
    libevdev_uinput_write_event(m_uinputDev,EV_SYN,SYN_REPORT,NULL);

    libevdev_uinput_write_event(m_uinputDev,EV_KEY,KEY_W,1);
    libevdev_uinput_write_event(m_uinputDev,EV_SYN,SYN_REPORT,NULL);

    libevdev_uinput_write_event(m_uinputDev,EV_KEY,KEY_W,0);
    libevdev_uinput_write_event(m_uinputDev,EV_SYN,SYN_REPORT,NULL);

    libevdev_uinput_destroy(m_uinputDev);

}
