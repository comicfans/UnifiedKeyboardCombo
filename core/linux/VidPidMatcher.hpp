/*
 * =====================================================================================
 *
 *       Filename:  VidPidMatcher.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/28/2014 04:47:56 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef VIDPIDMATCHER_HPP_Z0CHOEPD
#define VIDPIDMATCHER_HPP_Z0CHOEPD

#include "DeviceMatcher.hpp"
#include <string>
#include "CommonWild.hpp"

using std::string;

class VidPidMatcher :public DeviceMatcher
{
public:

    static constexpr const char* const CLASS_NAME="VidPidMatcher";

    enum Index{VID,PID};

    void setWild(Index index,const std::string& wild);

    virtual bool matchDevice(const InputDevice& inputDevice) const override final;

    virtual const char* const className()const override final{
        return CLASS_NAME;
    }

    string matchWild(Index index)const{return m_vidPidWild[index];}

    static constexpr const char ANY_WILD='*';

    bool operator==(const VidPidMatcher& rhs)const;

    virtual VidPidMatcher* deepClone()const override final;

protected:

    virtual void writeSelf(ptree& writeTo)const override final;

    virtual void readSelf(const ptree& readFrom) override final;

private:

    string m_vidPidWild[2]={MATCH_ALL_WILD,MATCH_ALL_WILD};
};


#endif /* end of include guard: VIDPIDMATCHER_HPP_Z0CHOEPD */

