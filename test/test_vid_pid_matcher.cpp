/*
 * =====================================================================================
 *
 *       Filename:  test_vid_pid_matcher.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年11月30日 07时42分59秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include "PlatformInc.hpp"
#include "linux/VidPidMatcher.hpp"

#include <boost/property_tree/ptree.hpp>

#include <gtest/gtest.h>

    
EvdevInputDevice input("00ac","89ac",0,0,"physcial","BUS_USB","fakeinputname");

TEST(VidPidMatcherTest,testVidEqual){

    VidPidMatcher matcher;

    matcher.setWild(VidPidMatcher::VID,"00ac");

    ASSERT_TRUE(matcher.matchDevice(input));
}

TEST(VidPidMatcherTest,testVidNotEqual){

    VidPidMatcher matcher;

    matcher.setWild(VidPidMatcher::VID,"09ac");

    ASSERT_FALSE(matcher.matchDevice(input));
}

TEST(VidPidMatcherTest,testVidWild){

    VidPidMatcher matcher;

    matcher.setWild(VidPidMatcher::VID,"00a*");

    ASSERT_TRUE(matcher.matchDevice(input));
}


TEST(VidPidMatcherTest,testSaveLoad){

    ptree writeTo;
    VidPidMatcher matcher;

    matcher.setWild(VidPidMatcher::VID,"99ac");
    matcher.setWild(VidPidMatcher::PID,"720b");

    DeviceMatcher::write(matcher,writeTo);

    DeviceMatcher *readBack=DeviceMatcher::read(writeTo);

    VidPidMatcher *cast=static_cast<VidPidMatcher*>(readBack);

    ASSERT_TRUE(*cast==matcher);
}
