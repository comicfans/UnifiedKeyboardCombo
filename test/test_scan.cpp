#include "PlatformInc.hpp"
#include <iostream>


#include <gtest/gtest.h>


TEST(ScanDeviceTest,scan){
    auto list=InputDevice::scanDevices();

    for(auto &dev:list){
#ifdef _WIN32
        std::wcout
#else
        std::cout
#endif
            <<dev->description()<<std::endl;
    }
 
}
