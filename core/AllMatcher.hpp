/*
 * =====================================================================================
 *
 *       Filename:  AllMatcher.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年11月29日 16时33分44秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef ALLMATCHER_HPP_AFPYLSVP
#define ALLMATCHER_HPP_AFPYLSVP

#include "DeviceMatcher.hpp"

class AllMatcher :public DeviceMatcher
{
public:

    static constexpr const char* const CLASS_NAME="AllMatcher";

    virtual const char * const className()const override final {
        return CLASS_NAME;
    }

    virtual bool matchDevice(const InputDevice& inputDevice) const override final{
        return true;
    }

    virtual AllMatcher* deepClone()const override final{
        return new AllMatcher;
    }

protected:

    //do nothing 
    virtual void writeSelf(ptree& writeTo) const override final {}

    //ignore
    virtual void readSelf(const ptree& readFrom) override final {}


};


#endif /* end of include guard: ALLMATCHER_HPP_AFPYLSVP */
