#ifndef HUBPORTMATCHER_HPP_9JE7UBXN
#define HUBPORTMATCHER_HPP_9JE7UBXN

#include "DeviceMatcher.hpp"

#include <limits>

class HubPortMatcher :public DeviceMatcher
{
public:

    static constexpr const char * const CLASS_NAME="HubPortMatcher";

    virtual const char* const className()const override final{
        return CLASS_NAME;
    }

    enum NumberType {HUB,PORT};

    enum RangeType{MIN_INCLUDE,MAX_INCLUDE};

    bool virtual matchDevice(const InputDevice& inputDevice)const override final;

    void setEqual(NumberType numberType,int number){
        return setRange(numberType,number,number);}

    void setRange(NumberType numberType,int minInclude,int maxInclude);

    int range(NumberType numberType,RangeType rangeType)const{
        return m_hubPortMinMax[numberType][rangeType];
    }

    virtual HubPortMatcher* deepClone()const override final;

protected:

    virtual void writeSelf(ptree& writeTo)const override final;

    virtual void readSelf(const ptree& readFrom) override final;


private:

    int m_hubPortMinMax[2][2]={
        {0,std::numeric_limits<int>::max()},
        {0,std::numeric_limits<int>::max()}};

};


#endif /* end of include guard: HUBPORTMATCHER_HPP_9JE7UBXN */
