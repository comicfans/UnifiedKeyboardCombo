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
#include "linux/EvdevMatcher.hpp"

#include <boost/property_tree/ptree.hpp>

#include <gtest/gtest.h>

    
EvdevInputDevice input(_T("fakedevice"),_T("00ac"),_T("89ac"),_T("physcial"),
        _T("BUS_USB"),_T("fakeinputname"));

TEST(EvdevMatcherTest,testVidEqual){

    EvdevMatcher matcher;

    matcher.setWild(EvdevMatcher::VID,"00ac");

    ASSERT_TRUE(matcher.matchDevice(input));
}

TEST(EvdevMatcherTest,testVidNotEqual){

    EvdevMatcher matcher;

    matcher.setWild(EvdevMatcher::VID,"09ac");

    ASSERT_FALSE(matcher.matchDevice(input));
}

TEST(EvdevMatcherTest,testVidWild){

    EvdevMatcher matcher;

    matcher.setWild(EvdevMatcher::VID,_T("00a*"));

    ASSERT_TRUE(matcher.matchDevice(input));
}


TEST(EvdevMatcherTest,testSaveLoad){

    TreeType writeTo;
    EvdevMatcher matcher;

    matcher.setWild(EvdevMatcher::VID,_T("99ac"));
    matcher.setWild(EvdevMatcher::PID,_T("720b"));

    DeviceMatcher::write(matcher,writeTo);

    DeviceMatcher *readBack=DeviceMatcher::read(writeTo);

    EvdevMatcher *cast=static_cast<EvdevMatcher*>(readBack);

    ASSERT_TRUE(*cast==matcher);
}
