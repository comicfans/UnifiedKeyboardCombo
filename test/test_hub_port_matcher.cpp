#include "PlatformInc.hpp"
#include "linux/HubPortMatcher.hpp"

#include <gtest/gtest.h>

    
EvdevInputDevice input("00ac","89ac",0,0,"physcial","BUS_USB","fakeinputname");

TEST(HubPortMatcherTest,testNumberEqual){

    HubPortMatcher matcher;

    matcher.setEqual(HubPortMatcher::HUB,0);
    matcher.setEqual(HubPortMatcher::PORT,0);

    ASSERT_TRUE(matcher.matchDevice(input));
}

TEST(HubPortMatcherTest,testNumberNotEqual){

    HubPortMatcher matcher;

    matcher.setEqual(HubPortMatcher::HUB,1);

    ASSERT_FALSE(matcher.matchDevice(input));

    matcher.setEqual(HubPortMatcher::HUB,0);
    matcher.setEqual(HubPortMatcher::PORT,1);

    ASSERT_FALSE(matcher.matchDevice(input));
}

TEST(HubPortMatcherTest,testRange){

    HubPortMatcher matcher;

    matcher.setRange(HubPortMatcher::HUB,3,5);

    ASSERT_FALSE(matcher.matchDevice(input));
}


