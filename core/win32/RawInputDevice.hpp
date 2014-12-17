/*
 * =====================================================================================
 *
 *       Filename:  RawInputDevice.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/28/2014 04:25:22 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef RAWINPUTDEVICE_HPP_2RUIAKFU
#define RAWINPUTDEVICE_HPP_2RUIAKFU

#include "StringType.hpp"
#include <vector>
#include <memory>
#include <windows.h>
#include "KeyMap.hpp"

using std::vector;

class RawInputDevice
{
public:
    enum DeviceType{MOUSE,KEYBOARD,HID};

    typedef std::vector<std::unique_ptr<RawInputDevice> > DeviceListType;

    static DeviceListType scanDevices();

    const StringType& vid()const{return m_vid;}
    const StringType& pid()const{return m_pid;}
    const StringType& name()const{return m_name;}
    const DeviceType deviceType()const{return m_deviceType;}
    
    StringType description()const;
    static std::vector<StringType> parseDeviceName(const StringType& name);

    bool configure(const vector<KeyMap> & keyMaps,bool disableNoneKeyEvent,
            bool disableUnmappedKey);
    HANDLE handle()const{return m_handle;}


    void nextRaw(HWND hwnd,RAWINPUT *rawInput,WPARAM wParam,LPARAM lParam);
    void nextKey(HHOOK hookHandle,UINT message,WPARAM wParam,LPARAM lParam);

private:

    RawInputDevice(HANDLE handle);
    StringType m_vid;
    StringType m_pid;
    StringType m_name;
    DeviceType m_deviceType;
     
    std::unique_ptr<uint8_t[]> m_keyMaps;
    HANDLE m_handle;

    bool m_disableNoneKeyEvent;
    bool m_disableUnmappedKey;
};


#endif /* end of include guard: RAWINPUTDEVICE_HPP_2RUIAKFU */
