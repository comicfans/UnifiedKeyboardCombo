#include <gtest/gtest.h>
#include "MultiMatcher.hpp"
#include "AllMatcher.hpp"
#include "PlatformInc.hpp"

EvdevInputDevice input("fakeinput","00ac","89ac","physcial","BUS_USB","fakeinputname");

TEST(MultiMatcherTest,testDefaultMatch){

    MultiMatcher matcher;

    ASSERT_TRUE(matcher.matchDevice(input));

}

TEST(MultiMatcherTest,testAndMatch){

    MultiMatcher matcher;

    matcher.addMatcher(new AllMatcher());

    ASSERT_TRUE(matcher.matchDevice(input));

    ASSERT_TRUE(matcher.matchDevice(input));

}

TEST(MultiMatcherTest,testSaveLoad){

}

