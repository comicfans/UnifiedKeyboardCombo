#ifndef EVDEVMATCHER_HPP_VNOEJBI0
#define EVDEVMATCHER_HPP_VNOEJBI0

#include "DeviceMatcher.hpp"
#include "Utility.hpp"
#include "StringType.hpp"
#include "TreeType.hpp"

class EvdevMatcher :public DeviceMatcher
{
public:
    static constexpr const StringType::value_type* const CLASS_NAME="EvdevMatcher";

    static constexpr int PROP_NUMBER=4;

    enum WildIndex{NAME,VID,PID,PHYSICAL};

    void setWild(WildIndex index,const StringType& wild);

    virtual bool matchDevice(const InputDevice& inputDevice) const override final;

    virtual const StringType::value_type* const className()const override final{
        return CLASS_NAME;
    }

    string wild(WildIndex index)const{return m_wild[index];}

    virtual EvdevMatcher* deepClone()const override final;


    bool operator==(const EvdevMatcher& rhs)const;

protected:

    virtual void writeSelf(TreeType& writeTo)const override final;

    virtual void readSelf(const TreeType& readFrom) override final;

private:

    string m_wild[PROP_NUMBER]={
        MATCH_ALL_WILD,MATCH_ALL_WILD,
        MATCH_ALL_WILD,MATCH_ALL_WILD};

};


#endif /* end of include guard: EVDEVMATCHER_HPP_VNOEJBI0 */

