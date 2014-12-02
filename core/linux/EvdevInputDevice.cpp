#include "EvdevInputDevice.hpp"

#include "input.h"

#include <dirent.h>
#include <fcntl.h>
#include <cstring>
#include <unistd.h>

#include <unordered_map>
#include <boost/assert.hpp>
#include <iostream>

#include "UinputKeyboard.hpp"
#include "Utility.hpp"

#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>
 
EvdevInputDevice::EvdevInputDevice(string name,string vid,string pid,
            string physical,string bus,string filename){
    m_name=name;
    m_vid=vid;
    m_pid=pid;
    m_physical=physical;
    m_bus=bus;
    m_fileName=filename;
}

  

std::unordered_map<int,string> createBusNames(){

    std::unordered_map<int,string> ret;

    ret[BUS_PCI]="BUS_PCI";
    ret[BUS_ISAPNP]="BUS_ISAPNP";
    ret[BUS_USB]="BUS_USB";
    ret[BUS_HIL]="BUS_HIL";
    ret[BUS_BLUETOOTH]="BUS_BLUETOOTH";
    ret[BUS_VIRTUAL]="BUS_VIRTUAL";
    ret[BUS_ISA]="BUS_ISA";
    ret[BUS_I8042]="BUS_I8042";
    ret[BUS_XTKBD]="BUS_XTKBD";
    ret[BUS_RS232]="BUS_RS232";
    ret[BUS_GAMEPORT]="BUS_GAMEPORT";
    ret[BUS_PARPORT]="BUS_PARPORT";
    ret[BUS_AMIGA]="BUS_AMIGA";
    ret[BUS_ADB]="BUS_ADB";
    ret[BUS_I2C]="BUS_I2C";
    ret[BUS_HOST]="BUS_HOST";
    ret[BUS_GSC]="BUS_GSC";
    ret[BUS_ATARI]="BUS_ATARI";
    ret[BUS_SPI]="BUS_SPI";

    return ret;
}

static const auto BUS_MAP_NAME=createBusNames();

string EvdevInputDevice::description()const{

    return 
        "Filename:"+m_fileName+ 
        ", Name: "+m_name +
        ", Bus:"+m_bus +
        ", PID: "+m_pid+
        ", VID:"+m_vid+
        ", Physical:"+m_physical;
}

static const char *DEV_INPUT_EVENT="/dev/input";

static const char *EVENT_DEV_NAME="event";

static int is_event_device(const struct dirent *dir) {
	return strncmp(EVENT_DEV_NAME, dir->d_name, 5) == 0;
}

EvdevInputDevice::EvdevInputDevice(const char * filename){

    log(INFO,"creating device from :",filename);

    m_evdevFd = open(filename, O_RDONLY|O_NONBLOCK);

    if (m_evdevFd < 0){
        log(DEBUG,"can not open file :",filename,",maybe you need root permission");
        return;
    }

    int rc=libevdev_new_from_fd(m_evdevFd,&m_evdev);

    if(rc<0){
        log(DEBUG,"can not open file :",filename,",maybe you need root permission");
        return ;
    }

    if (!libevdev_has_event_type(m_evdev,EV_KEY)){
        //not support EV_KEY
        log(INFO,filename," not support EV_KEY,skip it");
        
        libevdev_free(m_evdev);
        m_evdev=nullptr;
        return;
    }

    m_fileName=filename;
    m_name=libevdev_get_name(m_evdev);

    int busType=libevdev_get_id_bustype(m_evdev);

    auto it = BUS_MAP_NAME.find(busType);

    //some evdev input has no bus (uinput,HDA Intel MID Front Headphone)
    bool hasBus=(it!=BUS_MAP_NAME.end());
    m_bus=(hasBus?it->second:"");


    char fourChar[5]="0000";


    snprintf(fourChar,5,"%04x",libevdev_get_id_vendor(m_evdev));

    m_vid=fourChar;

    snprintf(fourChar,5,"%04x",libevdev_get_id_product(m_evdev));

    m_pid=fourChar;

    // uinput has no phys
        
    auto p=libevdev_get_phys(m_evdev);
    m_physical=p?p:"";

}

EvdevInputDevice::DeviceListType EvdevInputDevice::scanDevices(){

    DeviceListType ret;

	struct dirent **namelist;
	int i, ndev, devnum;
	char *filename;

	ndev = scandir(DEV_INPUT_EVENT, &namelist, is_event_device, versionsort);
	if (ndev <= 0)
		return ret;

	for (i = 0; i < ndev; i++)
	{
		char fname[64];

		snprintf(fname, sizeof(fname),
			 "%s/%s", DEV_INPUT_EVENT, namelist[i]->d_name);

    
        auto thisOne = unique_ptr<EvdevInputDevice>(
                new EvdevInputDevice(fname));
		
		free(namelist[i]);

        if(thisOne->m_evdev!=nullptr){

            log(INFO,thisOne->description()," created");
            ret.push_back(std::move(thisOne));
        }
	}

    free(namelist);

    return ret;
}



bool EvdevInputDevice::configure(const vector<KeyMap> & keyMaps,
        bool disableNonKeyEvent,bool disableUnmappedKey){

    m_keyMapCache.clear();

    for(auto &keyMap:keyMaps){
        //TODO invalid value ?
        log(TRACE,"map key ",keyMap.fromKey.c_str(),keyMap.toKey.c_str());
        m_keyMapCache[keyMap.fromKeyCode()]=keyMap.toKeyCode();
    }

    m_disableNonKeyEvent=disableNonKeyEvent;
    m_disableUnmappedKey=disableUnmappedKey;

    return grabAndPrepare();
}


bool EvdevInputDevice::processEvent(){

    input_event ev;
    int rc = libevdev_next_event(m_evdev, LIBEVDEV_READ_FLAG_NORMAL, &ev);

    if (rc == 0){

        if (ev.type==EV_KEY) {
            auto it=m_keyMapCache.find(ev.code);

            //mapped key
            if(it!=m_keyMapCache.end()){

                ev.code=it->second;
                log(TRACE,"map to key ",&ev);

                UinputKeyboard::instance().postKeyEvent(it->second,ev.value);

                return true;
            }

            //unmapped key
            if (!m_disableUnmappedKey) {
                log(TRACE,"passthrough unmapped key code,value :",&ev);
                UinputKeyboard::instance().postKeyEvent(ev.code,ev.value);
            }else{
                log(TRACE,"drop unmapped key : ",&ev);
            }
                
            return true;
        }


        if(m_disableNonKeyEvent){
            log(TRACE,"drop none EV_KEY event :",&ev);
            return true;
        }

        log(TRACE,"passthrough event :",&ev);

        BOOST_ASSERT(m_uinputDev);

        libevdev_uinput_write_event(m_uinputDev,ev.type,ev.code,ev.value);

        return true;
    }
       
            
    return (rc == 1 || rc == 0 || rc == -EAGAIN);
}

bool EvdevInputDevice::grabAndPrepare(){
    
    int rc=libevdev_grab(m_evdev,LIBEVDEV_GRAB);

    if(rc<0){
        return false;
    }

    if (m_disableNonKeyEvent ) {
        return true;
    }

    int needPassthroughType[]={
        EV_REL,
        EV_ABS,
        EV_SW ,
        EV_LED,
        EV_SND,
        EV_FF ,
        EV_PWR,
        EV_FF_STATUS
    };


    bool hasOtherEventType=false;
    for (int i = 0; i < sizeof(needPassthroughType)/sizeof(int); ++i){
        if(libevdev_has_event_type(m_evdev,needPassthroughType[i])){

            hasOtherEventType=true;
            break;
        }
    }

    //no need to create shadow input
    if (!hasOtherEventType || m_disableNonKeyEvent) {
        log(DEBUG,"device only support EV_KEY, no need to create shadow input ,",m_name.c_str());
        return true;
    }

    log(INFO,"will create shadow input of ",m_name.c_str());

    libevdev_set_name(m_evdev,("Non-Key event of "+m_name).c_str());

    rc=libevdev_uinput_create_from_device(m_evdev,
            LIBEVDEV_UINPUT_OPEN_MANAGED,&m_uinputDev);

    if (rc<0){
        return false;
    }

    return true;
}

EvdevInputDevice::~EvdevInputDevice(){

    if(m_uinputDev){

        libevdev_uinput_destroy(m_uinputDev);
    }

    if (m_evdev) {
        libevdev_grab(m_evdev,LIBEVDEV_UNGRAB);
        libevdev_free(m_evdev);
    }
        
    if (m_evdevFd>=0) {
        close(m_evdevFd);
    }
}

