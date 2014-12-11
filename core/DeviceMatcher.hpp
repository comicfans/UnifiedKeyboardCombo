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


#include "TreeType.hpp"
#include "StringType.hpp"

using std::unordered_map;
using std::function;
using std::shared_ptr;

class DeviceMatcher
{
public:

    virtual const StringType::value_type* const className()const = 0;

    virtual bool matchDevice(const InputDevice& inputDevice)const=0;

    virtual ~DeviceMatcher (){}

    virtual DeviceMatcher * deepClone()const=0;

    static void write(const DeviceMatcher &matcher,TreeType& writeTo);

    static DeviceMatcher *read(const TreeType& readFrom);

    template<typename T>
        static void registerSubType();

protected:

    virtual void writeSelf(TreeType& writeTo)const=0;

    virtual void readSelf(const TreeType& readFrom)=0;

private:

    static unordered_map<StringType,function<DeviceMatcher*(const TreeType&)> > s_createFunctions;

    static const bool GLOBAL_INIT;
};


#endif /* end of include guard: DEVICEMATCHER_HPP_LZAPOXCN */

