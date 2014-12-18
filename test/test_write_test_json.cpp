#include "Profile.hpp"

#include <boost/property_tree/json_parser.hpp>
#include "TreeType.hpp"

#include "linux/EvdevMatcher.hpp"

int main(int argc, char *argv[])
{

    Profile profile;

    EvdevMatcher *matcher=new EvdevMatcher;

    matcher->setWild(EvdevMatcher::VID,_T("0433"));
    matcher->setWild(EvdevMatcher::PID,_T("0004"));

    profile.setMatcher(matcher);

    
    KeyMap keyMap;

#ifdef _WIN32
    keyMap.fromKey="KEY_W";
    keyMap.toKey="KEY_P";
#else
    keyMap.fromKey=_T("W");
    keyMap.toKey=_T("P");
#endif

    profile.addKeyMap(keyMap);

    vector<Profile> list;

    list.push_back(profile);
    list.push_back(profile);

    TreeType root;

    Profile::writeList(list,root);

    write_json("config.json",root);
    
    return 0;
}
