#include "DeviceMatcher.hpp" 
#include "AllMatcher.hpp"

#include <boost/property_tree/ptree.hpp>

#include "GlobalInit.hpp"

unordered_map<string,function<DeviceMatcher*(const ptree&)> > DeviceMatcher::s_createFunctions;

const bool DeviceMatcher::GLOBAL_INIT=globalInit();

static const string CLASS_NAME_KEY("CLASS_NAME");

void DeviceMatcher::write(const DeviceMatcher &matcher,ptree& writeTo){

    writeTo.put(CLASS_NAME_KEY,matcher.className());

    matcher.writeSelf(writeTo);

}

DeviceMatcher *DeviceMatcher::read(const ptree& readFrom){
    
    auto className=readFrom.get<string>(CLASS_NAME_KEY);

    auto functionIt=s_createFunctions.find(className);

    if(functionIt==s_createFunctions.end()){
        return new AllMatcher();
    }

    return functionIt->second(readFrom);

}
