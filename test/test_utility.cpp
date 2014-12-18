#include <gtest/gtest.h>
#include "Utility.hpp"

#include "StringType.hpp"


StringType copyStrip(const StringType& input){
    StringType toStrip(input);
    stripWildString(toStrip);
    return toStrip;
}

TEST(UtilityTest,testStripNone){

    StringType orig(_T("ajsdfasdf"));

    StringType result=copyStrip(orig);

    ASSERT_EQ(orig,result);
 
}

TEST(UtilityTest,testStripNoneWithMatchAll){

    StringType orig(_T("a*a"));

    StringType result=copyStrip(orig);

    ASSERT_EQ(orig,result);
 
}

TEST(UtilityTest,testStripRemove){

    StringType orig("a**a");

    stripWildString(orig);

    ASSERT_EQ(orig,StringType(_T("a*a")));
 
}

TEST(UtilityTest,testMatchAny){
    ASSERT_TRUE(simpleWildMatch(_T("abc"),_T("*")));
    ASSERT_TRUE(simpleWildMatch(_T("abc"),_T("a*")));
    ASSERT_TRUE(simpleWildMatch(_T("abc"),_T("a*c")));
    ASSERT_TRUE(simpleWildMatch(_T("abc"),_T("*b*")));
    ASSERT_TRUE(simpleWildMatch(_T("abc"),_T("a*")));
    ASSERT_TRUE(simpleWildMatch(_T("abc"),_T("*c")));
    ASSERT_TRUE(simpleWildMatch(_T("abc"),_T("**")));
    ASSERT_FALSE(simpleWildMatch(_T("abc"),_T("bc")));
    ASSERT_FALSE(simpleWildMatch(_T("abc"),_T("c")));
    ASSERT_FALSE(simpleWildMatch(_T("abc"),_T("a")));
    ASSERT_FALSE(simpleWildMatch(_T("Power Button"),_T("*CHESEN*")));
}
