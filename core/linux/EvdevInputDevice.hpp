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
    EvdevInputDevice(string vid,string pid,
            int hubNumber,int portNumber,
            string physical,string bus,string filename);

    const std::string& vid()const{return m_vid;}
    const std::string& pid()const {return m_pid;}

    int hubNumber()const {return m_hubNumber;}

    int portNumber()const{return m_portNumber;}

    void setKeyMaps(const vector<KeyMap> & keyMaps );

    bool processEvent();

    string description()const;

    EvdevInputDevice(const EvdevInputDevice& rhs)=delete;
    EvdevInputDevice &operator=(const EvdevInputDevice& rhs)=delete;

    typedef vector<unique_ptr<EvdevInputDevice> > DeviceListType;

    static DeviceListType scanDevices();

    bool prepare();

    ~EvdevInputDevice();

private:

    EvdevInputDevice()=default;

    string m_pid;
    string m_vid;

    int m_hubNumber;
    int m_portNumber;

    //not used now
    string m_name;

    //not used now
    string m_physical;

    //not used now
    string m_bus;

    //not used now
    string m_fileName;

    unordered_map<unsigned int,unsigned int> m_keyMapCache;

    static constexpr int INVALID_FD=-1;

    int m_evdevFd=INVALID_FD;

    libevdev *m_evdev=nullptr;

    libevdev_uinput *m_uinputDev = nullptr;
};


#endif /* end of include guard: EVDEVINPUTDEVICE_HPP_02FXJUQZ */
