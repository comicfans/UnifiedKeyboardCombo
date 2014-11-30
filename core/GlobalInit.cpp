#include "DeviceMatcher.hpp"

#include "AllMatcher.hpp"
#include "NotMatcher.hpp"
#include "MultiMatcher.hpp"

#ifdef __linux
    #include "linux/HubPortMatcher.hpp"
    #include "linux/VidPidMatcher.hpp"
#elif __WIN32

#endif




template<typename T>
void DeviceMatcher::registerSubType(){
    s_createFunctions[T::CLASS_NAME]=[](const ptree& propTree)
            {
            DeviceMatcher *ret=new T();

            ret->readSelf(propTree);

            return ret;
            };

}

bool globalInit(){

    DeviceMatcher::registerSubType<AllMatcher>();
    DeviceMatcher::registerSubType<NotMatcher>();
    DeviceMatcher::registerSubType<MultiMatcher>();

#ifdef __linux
    DeviceMatcher::registerSubType<HubPortMatcher>();
    DeviceMatcher::registerSubType<VidPidMatcher>();
#elif __WIN32

#endif

    return true;
}

