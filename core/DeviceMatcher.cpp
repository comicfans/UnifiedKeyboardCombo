#include "DeviceMatcher.hpp" 
#include "AllMatcher.hpp"

#include <boost/property_tree/ptree.hpp>

#include "GlobalInit.hpp"

unordered_map<StringType,function<DeviceMatcher*(const TreeType&)> > DeviceMatcher::s_createFunctions;

const bool DeviceMatcher::GLOBAL_INIT=globalInit();

static const StringType CLASS_NAME_KEY(_T("CLASS_NAME"));

void DeviceMatcher::write(const DeviceMatcher &matcher,TreeType& writeTo){

    writeTo.put(CLASS_NAME_KEY,matcher.className());

    matcher.writeSelf(writeTo);

}

DeviceMatcher *DeviceMatcher::read(const TreeType& readFrom){
    
    auto className=readFrom.get<StringType>(CLASS_NAME_KEY);

    auto functionIt=s_createFunctions.find(className);

    if(functionIt==s_createFunctions.end()){
        return new AllMatcher();
    }

    return functionIt->second(readFrom);

}
