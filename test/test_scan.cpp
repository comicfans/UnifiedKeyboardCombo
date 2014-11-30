#include "PlatformInc.hpp"
#include <iostream>


#include <gtest/gtest.h>


TEST(ScanDeviceTest,scan){
    auto list=InputDevice::scanDevices();

    for(auto &dev:list){
        std::cout<<dev->description()<<'\n';
    }
 
}
