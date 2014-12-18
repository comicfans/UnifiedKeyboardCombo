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

 
#include "Utility.hpp"

static constexpr int WIN_KEY_CNT=256;
    
RawInputDevice::DeviceListType RawInputDevice::scanDevices(){
    UINT number;

    auto value=GetRawInputDeviceList(nullptr,&number,sizeof(RAWINPUTDEVICELIST));
    if (value==UINT(-1)){
        ukc_log(UKC_ERROR,_T("error"),GetLastError());
        return DeviceListType();
    }
    if(number==0){
        ukc_log(UKC_INFO,_T("no rawinput device found"),_T(""));
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
        std::unique_ptr<RawInputDevice> thisOne(new RawInputDevice(list[i].hDevice));
        if(thisOne->m_handle){
            ret.push_back(std::move(thisOne));
        }
    }
    return ret;
}

StringType RawInputDevice::description()const{

    auto ret=_T("name : ")+m_name;       
    if(!m_vid.empty()){
        ret+=_T(",vid:")+m_vid+_T(",pid:")+m_pid;
    }
    return ret;
}

static constexpr const StringType::value_type * const DEVICE_DESC=_T("DeviceDesc");

RawInputDevice::RawInputDevice(HANDLE device){

    m_keyMaps.reset(new uint8_t[WIN_KEY_CNT]);

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
        ukc_log(UKC_ERROR,_T("error when getting device name : "),GetLastError());
        m_handle=nullptr;
        return;
    }
    RID_DEVICE_INFO info;

    dataSize=sizeof(RID_DEVICE_INFO);
    value=GetRawInputDeviceInfo(device,RIDI_DEVICEINFO,&info,&dataSize);

    if (value==UINT(-1))
    {
        ukc_log(UKC_ERROR,_T("error when getting device name : "),GetLastError());
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
                LR"(System\CurrentControlSet\Enum\%s\%s\%s)")  
            %infos[0]%infos[1]%infos[2]).str();    

    DWORD size;



    auto er=RegGetValue(HKEY_LOCAL_MACHINE,keyPath.data(),DEVICE_DESC,RRF_RT_REG_SZ,nullptr,nullptr,&size);
    if(er!=decltype(er)(ERROR_SUCCESS)){
        ukc_log(UKC_ERROR,_T("error when opening reg to get device info"),er);
        m_handle=nullptr;
        return;
    }

    //bit enough ?

    auto byteSize=size*2;
    StringType::value_type descBuff[byteSize/sizeof(StringType::value_type)*2];
    er=RegGetValue(HKEY_LOCAL_MACHINE,keyPath.data(),DEVICE_DESC,
                RRF_RT_REG_SZ,nullptr,descBuff,&byteSize);
    if(er!=decltype(er)(ERROR_SUCCESS)){
        ukc_log(UKC_ERROR,_T("error when opening reg to get device info"),er);
        m_handle=nullptr;
        return;
    }
    m_name=descBuff;
    m_name=m_name.substr(m_name.find(';')+1);
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

bool RawInputDevice::RawInputDevice::configure(const vector<KeyMap> &keyMaps,
        bool disableNoneKeyEvent,bool disableUnmappedKey){

    m_disableUnmappedKey=disableUnmappedKey;

    if(m_disableUnmappedKey){
        std::fill_n(m_keyMaps.get(),m_keyMaps.get()+WIN_KEY_CNT,0);
    }else{
        for (int i = 0; i < WIN_KEY_CNT; ++i) {
            //linux key map is dense map
            m_keyMaps[i]=i;
        }
    }

    for(auto &keyMap:keyMaps){
        ukc_log(UKC_TRACE,_T("map key "),keyMap.fromKey.c_str(),keyMap.toKey.c_str());

        int fromKeyCode=keyMap.fromKeyCode();

        if (fromKeyCode==-1) {
            ukc_log(UKC_ERROR,_T("bad from key name"),keyMap.fromKey.c_str());
            continue;
        }

        int toKeyCode=keyMap.toKeyCode();

        if (toKeyCode==-1) {
            ukc_log(UKC_ERROR,_T("bad to key name"),keyMap.toKey.c_str());
            continue;
        }

        BOOST_ASSERT(
                fromKeyCode>=0 && fromKeyCode<WIN_KEY_CNT && 
                toKeyCode>=0 && toKeyCode<WIN_KEY_CNT);

        m_keyMaps[fromKeyCode]=toKeyCode;
    }

    m_disableNoneKeyEvent=disableNoneKeyEvent;

    return true;

}

    
void RawInputDevice::nextRaw(HWND hwnd,RAWINPUT *raw,WPARAM wParam,LPARAM lParam){
    // Get the virtual key code of the key and report it
    USHORT virtualKeyCode = raw->data.keyboard.VKey;
    USHORT keyPressed = raw->data.keyboard.Flags & RI_KEY_BREAK ? 0 : 1;
    WCHAR text[128];
    swprintf (text, 128, L"Raw Input: %X (%d)\n", virtualKeyCode, keyPressed);
    OutputDebugString (text);

    // Prepare string buffer for the device name


}

    
void RawInputDevice::nextKey(HHOOK hookHandle,UINT message,WPARAM wParam,LPARAM lParam){
    USHORT virtualKeyCode=wParam;

    WCHAR text[128];
    USHORT keyPressed= (message==WM_KEYDOWN || message==WM_SYSKEYDOWN);
    swprintf (text, 128, L"key : %X (%d)\n", virtualKeyCode, keyPressed);
    OutputDebugString (text);

}
