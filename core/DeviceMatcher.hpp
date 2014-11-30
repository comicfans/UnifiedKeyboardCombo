/*
 * =====================================================================================
 *
 *       Filename:  devicematcher.hpp
 *
 *    Description:  
 *
 *
 *        Version:  1.0
 *        Created:  11/28/2014 04:13:31 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef DEVICEMATCHER_HPP_LZAPOXCN
#define DEVICEMATCHER_HPP_LZAPOXCN

#include "PlatformDef.hpp"

#include <memory>
#include <functional>
#include <unordered_map>
#include <string>
#include <boost/property_tree/ptree_fwd.hpp>


using boost::property_tree::ptree;
using std::unordered_map;
using std::string;
using std::function;
using std::shared_ptr;

class DeviceMatcher
{
public:

    virtual const char* const className()const = 0;

    virtual bool matchDevice(const InputDevice& inputDevice)const=0;

    virtual ~DeviceMatcher (){}

    virtual DeviceMatcher * deepClone()const=0;

    static void write(const DeviceMatcher &matcher,ptree& writeTo);

    static DeviceMatcher *read(const ptree& readFrom);

    template<typename T>
        static void registerSubType();

protected:

    virtual void writeSelf(ptree& writeTo)const=0;

    virtual void readSelf(const ptree& readFrom)=0;

private:

    static unordered_map<string,function<DeviceMatcher*(const ptree&)> > s_createFunctions;

    static const bool GLOBAL_INIT;
};


#endif /* end of include guard: DEVICEMATCHER_HPP_LZAPOXCN */

