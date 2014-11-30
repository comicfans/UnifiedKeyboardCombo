#include "EvdevInputDevice.hpp"

#include "input.h"

#include <dirent.h>
#include <fcntl.h>
#include <cstring>
#include <unistd.h>

#include <unordered_map>
#include <iostream>

#include "UinputKeyboard.hpp"

#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>
 
EvdevInputDevice::EvdevInputDevice(string vid,string pid,
            int hubNumber,int portNumber,
            string physical,string bus,string filename){
    m_vid=vid;
    m_pid=pid;
    m_hubNumber=hubNumber;
    m_portNumber=portNumber;
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
        "Name: "+m_name +
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
		int fd = -1;
		char name[256] = "???";

		snprintf(fname, sizeof(fname),
			 "%s/%s", DEV_INPUT_EVENT, namelist[i]->d_name);

		fd = open(fname, O_RDONLY);

		if (fd < 0){
			continue;
        }

	
		int rc=ioctl(fd, EVIOCGNAME(sizeof(name)), name);

        if (rc < 0){
            //can not get name
            continue;
        }

        auto thisOne = unique_ptr<EvdevInputDevice>(new EvdevInputDevice());
	
        thisOne->m_fileName=fname;

        thisOne->m_name=name;

        unsigned short id[4];

        rc=ioctl(fd, EVIOCGID, id);

        if(rc<0){
            if(errno != ENOENT){
                continue;
            }
        }

        thisOne->m_bus=BUS_MAP_NAME.find(id[ID_BUS])->second;

        char fourChar[5]="0000";
        snprintf(fourChar,5,"%04x",id[ID_VENDOR]);
        thisOne->m_vid=fourChar;

        snprintf(fourChar,5,"%04x",id[ID_PRODUCT]);
        thisOne->m_pid=fourChar;

	
        char physicalBuf[256];
        memset(physicalBuf,256,0);

        rc = ioctl(fd, EVIOCGPHYS(sizeof(physicalBuf) - 1), physicalBuf);
        if(rc<0){
            // uinput has no phys
            thisOne->m_physical="uinput";
        }else{
            thisOne->m_physical=physicalBuf;
        }


		close(fd);
		free(namelist[i]);

        ret.push_back(std::move(thisOne));
	}

    free(namelist);

    return ret;
}



void EvdevInputDevice::setKeyMaps(const vector<KeyMap> & keyMaps){
    m_keyMapCache.clear();

    for(auto &keyMap:keyMaps){
        //TODO invalid value ?
        m_keyMapCache[keyMap.fromKeyCode()]=keyMap.toKeyCode();
    }
}


bool EvdevInputDevice::processEvent(){

    input_event ev;
    int rc = libevdev_next_event(m_evdev, LIBEVDEV_READ_FLAG_NORMAL, &ev);


    if (rc == 0){

        bool passthrough=true;

        if (ev.type==EV_KEY)
        {
            auto it=m_keyMapCache.find(ev.code);
            if(it!=m_keyMapCache.end()){
                passthrough=false;

                UinputKeyboard::instance().postKeyEvent(it->second,ev.value);

            }
        }





        if (passthrough) {

            libevdev_uinput_write_event(m_uinputDev,ev.type,ev.code,ev.value);

        }
    }
       
            
    return (rc == 1 || rc == 0 || rc == -EAGAIN);
}

bool EvdevInputDevice::prepare(){

    m_evdevFd=open(m_fileName.c_str(),O_RDONLY|O_NONBLOCK);

    if (m_evdevFd<0){
        return false;
    }

    int rc=libevdev_new_from_fd(m_evdevFd,&m_evdev);

    if(rc<0){
        return false;
    }

    rc=libevdev_grab(m_evdev,LIBEVDEV_GRAB);

    if(rc<0){
        return false;
    }


    libevdev_set_name(m_evdev,("shadow of "+m_name).c_str());

    rc=libevdev_uinput_create_from_device(m_evdev,
            LIBEVDEV_UINPUT_OPEN_MANAGED,&m_uinputDev);

    return true;
}

EvdevInputDevice::~EvdevInputDevice(){
    if (m_evdev)
    {
        libevdev_grab(m_evdev,LIBEVDEV_UNGRAB);
        libevdev_free(m_evdev);
        close(m_evdevFd);
    }
}
