#ifndef EVDEVINPUTDEVICE_HPP_02FXJUQZ
#define EVDEVINPUTDEVICE_HPP_02FXJUQZ

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "KeyMap.hpp"

using std::string;
using std::unique_ptr;
using std::vector;
using std::unordered_map;

struct libevdev;
struct libevdev_uinput;

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

private:

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


    unordered_map<unsigned int,unsigned int> m_keyMapCache;

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

};


#endif /* end of include guard: EVDEVINPUTDEVICE_HPP_02FXJUQZ */
