#ifndef EVDEVINPUTDEVICE_HPP_02FXJUQZ
#define EVDEVINPUTDEVICE_HPP_02FXJUQZ

#include <string>
#include <vector>
#include <memory>

#include "KeyMap.hpp"

using std::string;
using std::unique_ptr;
using std::vector;

struct libevdev;
struct libevdev_uinput;

static constexpr const char * const EVENT_INPUT_PATH="/dev/input/";

class EvdevInputDevice 
{
public:

    //only for test purpose
    EvdevInputDevice(string name,string vid,string pid,
            string physical,string bus,string filename);

    const string& vid()const{return m_vid;}
    const string& pid()const {return m_pid;}
    const string& name()const{return m_name;}
    const string& physical()const{return m_physical;}

    bool processEvent();

    string description()const;

    EvdevInputDevice(const EvdevInputDevice& rhs)=delete;
    EvdevInputDevice &operator=(const EvdevInputDevice& rhs)=delete;

    typedef vector<unique_ptr<EvdevInputDevice> > DeviceListType;

    static DeviceListType scanDevices();

    bool configure(const vector<KeyMap> & keyMaps,
            bool disableNonKeyEvent,bool disableUnmappedKey);


    ~EvdevInputDevice();

    int evdevFd()const {return m_evdevFd;}

    static EvdevInputDevice * tryCreateNew(const char * name);

    bool registerPoll(int epollFd);

private:

    EvdevInputDevice();

    EvdevInputDevice(const char * filename);

    bool grabAndPrepare();

    string m_pid;
    string m_vid;


    //not used now
    string m_name;

    //not used now
    string m_physical;

    //not used now
    string m_bus;

    //not used now
    string m_fileName;

    string m_uniq;

    unique_ptr<uint16_t[]> m_keyMaps;

    static constexpr int INVALID_FD=-1;

    int m_evdevFd=INVALID_FD;

    bool m_disableUnmappedKey=false;

    bool m_disableNonKeyEvent=false;

    bool m_mayPassthrough;

    libevdev *m_evdev=nullptr;

    /**
     * @brief if support other events ,create shadow uinput to passthrough
     * */
    libevdev_uinput *m_uinputDev = nullptr;

    int m_epollFd=-1;

    bool valid()const;
};


#endif /* end of include guard: EVDEVINPUTDEVICE_HPP_02FXJUQZ */
