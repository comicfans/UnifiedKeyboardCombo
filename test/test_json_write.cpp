#include "MultiMatcher.hpp"
#include "AllMatcher.hpp"
#include "NotMatcher.hpp"
#include "TreeType.hpp"

//#include <gtest/gtest.h>

#include <boost/property_tree/json_parser.hpp>

int main(int argc, char *argv[])
{

    MultiMatcher top;

    NotMatcher *notMatch=new NotMatcher();

    notMatch->setMatcher(new AllMatcher());

    top.addMatcher(notMatch);
    
    top.addMatcher(new AllMatcher());

    TreeType tree;

    DeviceMatcher::write(top,tree);

    write_json("out.json",tree);

    TreeType readBack;
    read_json("out.json",readBack);

    DeviceMatcher *readBackMatcher=DeviceMatcher::read(readBack);

    
    return 0;
}

