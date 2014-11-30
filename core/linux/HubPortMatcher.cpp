#include "HubPortMatcher.hpp"
#include "PlatformInc.hpp"

#include <boost/property_tree/ptree.hpp>
    

bool HubPortMatcher::matchDevice(const InputDevice& inputDevice)const{


    return 
        inputDevice.hubNumber()>=m_hubPortMinMax[HUB][0]&&
        inputDevice.hubNumber()<=m_hubPortMinMax[HUB][1]&&
        inputDevice.portNumber()>=m_hubPortMinMax[PORT][0]&&
        inputDevice.portNumber()<=m_hubPortMinMax[PORT][1];
}

    
void HubPortMatcher::setRange(NumberType numberType,
        int minInclude,int maxInclude){

    if (minInclude>maxInclude)
    {
        return;
    }

    m_hubPortMinMax[numberType][0]=minInclude;
    m_hubPortMinMax[numberType][1]=maxInclude;
}

static const string PROP_KEYS[4]={
    "hub.min","hub.max","port.min","port.max"
};
    
void HubPortMatcher::writeSelf(ptree& writeTo)const{

    for(int i=0;i<4;++i){
        writeTo.put(PROP_KEYS[i],m_hubPortMinMax[i/2][i%2]);
    }

}

void HubPortMatcher::readSelf(const ptree& readFrom) {

    for(int i=0;i<4;++i){
        m_hubPortMinMax[i/2][i%2]=readFrom.get<int>(PROP_KEYS[i]);
    }
}


    
HubPortMatcher* HubPortMatcher::deepClone()const {

    HubPortMatcher *clone=new HubPortMatcher;

    std::copy_n(&m_hubPortMinMax[0][0],4,&clone->m_hubPortMinMax[0][0]);

    return clone;

}

