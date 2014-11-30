#include "Profile.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "linux/VidPidMatcher.hpp"

int main(int argc, char *argv[])
{

    Profile profile;

    VidPidMatcher *matcher=new VidPidMatcher;

    matcher->setWild(VidPidMatcher::VID,"0433");
    matcher->setWild(VidPidMatcher::PID,"0004");

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
