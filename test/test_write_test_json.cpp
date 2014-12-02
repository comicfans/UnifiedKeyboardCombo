#include "Profile.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "linux/EvdevMatcher.hpp"

int main(int argc, char *argv[])
{

    Profile profile;

    EvdevMatcher *matcher=new EvdevMatcher;

    matcher->setWild(EvdevMatcher::VID,"0433");
    matcher->setWild(EvdevMatcher::PID,"0004");

    profile.setMatcher(matcher);

    
    KeyMap keyMap;

    keyMap.fromKey="KEY_W";
    keyMap.toKey="KEY_P";

    profile.addKeyMap(keyMap);

    vector<Profile> list;

    list.push_back(profile);

    ptree root;

    Profile::writeList(list,root);

    write_json("config.json",root);
    
    return 0;
}
