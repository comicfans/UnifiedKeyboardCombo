/*
 * =====================================================================================
 *
 *       Filename:  RawInputDevice.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年12月12日 20时57分32秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include "RawInputDevice.hpp"

#include <winuser.h>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
    
RawInputDevice::DeviceListType RawInputDevice::scanDevices(){

    auto number=GetRawInputDeviceList(nullptr,nullptr,0);
    if (number!=0){
        
        return DeviceListType();
    }

    RAWINPUTDEVICELIST list[number*5];
    UINT numberDevices;
    //WTF? why OS didn't know its size? 
    number=GetRawInputDeviceList(list,&numberDevices,sizeof(RAWINPUTDEVICELIST));
    if (number<=0)
    {
        return DeviceListType();
    }
    DeviceListType ret;

    for (decltype(number) i = 0; i < number; ++i)
    {
    }
    return ret;
}

StringType RawInputDevice::description()const{
    return _T("name : ")+m_name+_T(",vid:")+m_vid+_T(",pid:")+m_pid;
}

static constexpr const StringType::value_type * const DEVICE_DESC=_T("DeviceDesc");

RawInputDevice::RawInputDevice(HANDLE device){

    m_handle=device;

    UINT dataSize;
    auto value=GetRawInputDeviceInfo(device,RIDI_DEVICENAME,NULL,&dataSize);

    //WTF ?
    if (value==UINT(-1))
    {
        m_handle=nullptr;
        return;
    }

    StringType::value_type nameBuff[dataSize+1];

    value=GetRawInputDeviceInfo(device,RIDI_DEVICENAME,nameBuff,&dataSize);

    if (value==UINT(-1))
    {
        m_handle=nullptr;
        return;
    }
    RID_DEVICE_INFO info;

    dataSize=sizeof(RID_DEVICE_INFO);
    value=GetRawInputDeviceInfo(device,RIDI_DEVICEINFO,&info,&dataSize);

    if (value==UINT(-1))
    {
        m_handle=nullptr;
        return ;
    }
    m_deviceType=DeviceType(info.dwType);

    auto infos=parseDeviceName(nameBuff);
    if (infos.empty())
    {
        m_handle=nullptr;
        return;
    }

    auto guid=infos.back();
    auto classCode=infos.front();

    StringType keyPath;

    if(classCode==_T("HID")){

        std::vector<StringType> vidPidSplit;
        boost::split(vidPidSplit,infos[1],boost::is_any_of(_T("&")),boost::token_compress_on);

        m_vid=vidPidSplit[0].substr(4,4);
        m_pid=vidPidSplit[1].substr(4,4);
        //TODO
    }

        
    auto &regPath=infos[1];
    auto &idPath=infos[2];

    keyPath=(boost::wformat(
                _T("(System\CurrentControlSet\Enum\%0%\%1%\%2%)"))
            %_T("ACPI")%infos[1]%infos[2]).str();    

    DWORD size;
    if(RegGetValue(HKEY_LOCAL_MACHINE,keyPath.data(),DEVICE_DESC,RRF_RT_REG_EXPAND_SZ,nullptr,nullptr,&size)!=ERROR_SUCCESS){
        m_handle=nullptr;
        return;
    }

    //bit enough ?

    auto byteSize=size*2;
    StringType::value_type descBuff[byteSize/sizeof(StringType::value_type)*2];
    if(RegGetValue(HKEY_LOCAL_MACHINE,keyPath.data(),DEVICE_DESC,
                RRF_RT_REG_EXPAND_SZ,nullptr,descBuff,&byteSize)!=ERROR_SUCCESS){
        m_handle=nullptr;
        return;
    }
    m_name=descBuff;
}

std::vector<StringType> RawInputDevice::parseDeviceName(const StringType& name){ 
    auto lastSlashPos=name.find_last_of(_T('\\'));
    if (lastSlashPos==StringType::npos)
    {
        
        return std::vector<StringType>();
    }
    auto slashStripped=name.substr(lastSlashPos+1);
    std::vector<StringType> result;
    boost::split(result,slashStripped,boost::is_any_of(_T("#")),
            boost::token_compress_on);
    return result;
}
