#include "MultiMatcher.hpp"
#include "AllMatcher.hpp"
#include "NotMatcher.hpp"

//#include <gtest/gtest.h>

#include <boost/property_tree/json_parser.hpp>

int main(int argc, char *argv[])
{

    MultiMatcher top;

    NotMatcher *notMatch=new NotMatcher();

    notMatch->setMatcher(new AllMatcher());

    top.addMatcher(notMatch);
    
    top.addMatcher(new AllMatcher());

    ptree tree;

    DeviceMatcher::write(top,tree);

    write_json("out.json",tree);

    ptree readBack;
    read_json("out.json",readBack);

    DeviceMatcher *readBackMatcher=DeviceMatcher::read(readBack);

    
    return 0;
}

