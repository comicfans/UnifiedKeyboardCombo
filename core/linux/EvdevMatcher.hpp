#ifndef EVDEVMATCHER_HPP_VNOEJBI0
#define EVDEVMATCHER_HPP_VNOEJBI0

#include "DeviceMatcher.hpp"
#include "Utility.hpp"

class EvdevMatcher :public DeviceMatcher
{
public:
    static constexpr const char* const CLASS_NAME="EvdevMatcher";

    static constexpr int PROP_NUMBER=4;

    enum WildIndex{NAME,VID,PID,PHYSICAL};

    void setWild(WildIndex index,const std::string& wild);

    virtual bool matchDevice(const InputDevice& inputDevice) const override final;

    virtual const char* const className()const override final{
        return CLASS_NAME;
    }

    string wild(WildIndex index)const{return m_wild[index];}

    virtual EvdevMatcher* deepClone()const override final;


    bool operator==(const EvdevMatcher& rhs)const;

protected:

    virtual void writeSelf(ptree& writeTo)const override final;

    virtual void readSelf(const ptree& readFrom) override final;

private:

    string m_wild[PROP_NUMBER]={
        MATCH_ALL_WILD,MATCH_ALL_WILD,
        MATCH_ALL_WILD,MATCH_ALL_WILD};

};


#endif /* end of include guard: EVDEVMATCHER_HPP_VNOEJBI0 */

