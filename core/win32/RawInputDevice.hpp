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
private:

    RawInputDevice(HANDLE handle);
    StringType m_vid;
    StringType m_pid;
    StringType m_name;
    DeviceType m_deviceType;
     
    HANDLE m_handle;
};


#endif /* end of include guard: RAWINPUTDEVICE_HPP_2RUIAKFU */
