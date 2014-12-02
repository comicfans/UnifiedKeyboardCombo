#include <gtest/gtest.h>
#include "Utility.hpp"

#include <string>

using std::string;

string copyStrip(const string& input){
    string toStrip(input);
    stripWildString(toStrip);
    return toStrip;
}

TEST(UtilityTest,testStripNone){

    string orig("ajsdfasdf");

    string result=copyStrip(orig);

    ASSERT_EQ(orig,result);
 
}

TEST(UtilityTest,testStripNoneWithMatchAll){

    string orig("a*a");

    string result=copyStrip(orig);

    ASSERT_EQ(orig,result);
 
}

TEST(UtilityTest,testStripRemove){

    string orig("a**a");

    stripWildString(orig);

    ASSERT_EQ(orig,string("a*a"));
 
}

TEST(UtilityTest,testMatchAny){
    ASSERT_TRUE(simpleWildMatch("abc","*"));
    ASSERT_TRUE(simpleWildMatch("abc","a*"));
    ASSERT_TRUE(simpleWildMatch("abc","a*c"));
    ASSERT_TRUE(simpleWildMatch("abc","*b*"));
    ASSERT_TRUE(simpleWildMatch("abc","a*"));
    ASSERT_TRUE(simpleWildMatch("abc","*c"));
    ASSERT_TRUE(simpleWildMatch("abc","**"));
    ASSERT_FALSE(simpleWildMatch("abc","bc"));
    ASSERT_FALSE(simpleWildMatch("abc","c"));
    ASSERT_FALSE(simpleWildMatch("abc","a"));
    ASSERT_FALSE(simpleWildMatch("Power Button","*CHESEN*"));
}
