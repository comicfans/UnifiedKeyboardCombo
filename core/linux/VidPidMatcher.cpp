#include "VidPidMatcher.hpp"

#include <algorithm>

#include <boost/assert.hpp>
#include <boost/property_tree/ptree.hpp>

#include "PlatformInc.hpp"

    
static const int VID_PID_LENGTH=4;

void VidPidMatcher::setWild(Index index,const string& wild){

    m_vidPidWild[index]=wild;
    m_vidPidWild[index].resize(VID_PID_LENGTH);

}

static bool wildMatch(const string& toTest,const string& wild){

    BOOST_ASSERT(wild.length()==4);

    for (int i = 0; i < VID_PID_LENGTH; ++i)
    {
        if (wild[i]==VidPidMatcher::ANY_WILD){
            continue;
        }

        if (toTest[i]!=wild[i]){
            return false;
        }
    }

    return true;
}

bool VidPidMatcher::matchDevice(const InputDevice& inputDevice) const{

    return 
        wildMatch(inputDevice.vid(),m_vidPidWild[VID])&&
        wildMatch(inputDevice.pid(),m_vidPidWild[PID]);

}

static const string PROP_KEYS[2]={"VID","PID"};

void VidPidMatcher::writeSelf(ptree& writeTo)const {

    for (int i = 0; i < 2; ++i){
        writeTo.put(PROP_KEYS[i],m_vidPidWild[i]);
    }
}

    
void VidPidMatcher::readSelf(const ptree& readFrom) {

    for (int i = 0; i < 2; ++i){
        m_vidPidWild[i]=readFrom.get(PROP_KEYS[i],m_vidPidWild[i]);
    }
}


    
bool VidPidMatcher::operator==(const VidPidMatcher& rhs)const{

    return std::equal(m_vidPidWild,m_vidPidWild+2,rhs.m_vidPidWild);

}

    
VidPidMatcher* VidPidMatcher::deepClone()const {

    VidPidMatcher *clone=new VidPidMatcher;

    std::copy_n(m_vidPidWild,2,clone->m_vidPidWild);

    return clone;
}

