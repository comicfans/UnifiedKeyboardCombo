#include "EvdevMatcher.hpp"

#include "Utility.hpp"
#include "PlatformInc.hpp"

#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
    
void EvdevMatcher::setWild(WildIndex index,const StringType& wild){

    StringType & toOperate=m_wild[index];

    toOperate=wild;

    stripWildString(toOperate);

    if (index==PID || index==VID){
        toOperate.resize(std::min<int>(4,wild.length()));
    }
}


bool EvdevMatcher::matchDevice(const InputDevice& inputDevice) const{

    return 
        simpleWildMatch(inputDevice.vid(),m_wild[VID])&&
        simpleWildMatch(inputDevice.pid(),m_wild[PID])&&
        simpleWildMatch(inputDevice.name(),m_wild[NAME])&&
        simpleWildMatch(inputDevice.physical(),m_wild[PHYSICAL]);


}


static const StringType WILD_KEYS[EvdevMatcher::PROP_NUMBER]=
{"NAME","VID","PID","PHYSICAL"};

void EvdevMatcher::writeSelf(TreeType& writeTo)const {

    for (int i = 0; i < PROP_NUMBER; ++i){
        writeTo.put(WILD_KEYS[i],m_wild[i]);
    }
}

    
void EvdevMatcher::readSelf(const TreeType& readFrom) {

    for (int i = 0; i < PROP_NUMBER; ++i){
        if(boost::optional<StringType> value=
                readFrom.get_optional<StringType>(WILD_KEYS[i])){
            m_wild[i]=*value;
        }else{
            m_wild[i]=MATCH_ALL_WILD;
        }
    }
}


    
bool EvdevMatcher::operator==(const EvdevMatcher& rhs)const{

    return std::equal(m_wild,m_wild+4,rhs.m_wild);

}

    
EvdevMatcher* EvdevMatcher::deepClone()const {

    EvdevMatcher *clone=new EvdevMatcher;

    std::copy_n(m_wild,PROP_NUMBER,clone->m_wild);

    return clone;
}

