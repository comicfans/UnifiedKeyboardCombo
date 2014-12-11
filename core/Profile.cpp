#include "Profile.hpp"

#include "DeviceMatcher.hpp"
#include "NotMatcher.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/optional.hpp>

Profile::Profile(){
    m_matcher.reset(new NotMatcher);
}

Profile::Profile(const Profile& rhs){

    operator=(rhs);
}

Profile& Profile::operator=(const Profile& rhs){

    m_matcher.reset(rhs.m_matcher->deepClone());

    m_keyMaps=rhs.m_keyMaps;

    m_disableUnmappedKey=rhs.m_disableUnmappedKey;
    m_disableNonKeyEvent=rhs.m_disableNonKeyEvent;

    return *this;
}

static const string MATCHER_KEY="MATCHER";

static const string KEY_MAPS_KEY="KEY_MAPS";

static const string FROM_KEY="FROM";

static const string DESCRIPTION_KEY="DESCRIPTION";
static const string NAME_KEY="NAME";

static const string TO_KEY="TO";

static const string DISABLE_NON_KEY_EVENT_KEY="DISABLE_NON_KEY_EVENT";

static const string DISABLE_UNMAPPED_KEY_KEY="DISABLE_UNMAPPED_KEY";

void Profile::read(const ptree& readFrom){

    m_name=readFrom.get<StringType>(NAME_KEY);
    m_description=readFrom.get<StringType>(DESCRIPTION_KEY);

    m_matcher.reset();
    m_keyMaps.clear();


    auto &matcherTree=readFrom.get_child(MATCHER_KEY);

    m_matcher.reset(DeviceMatcher::read(matcherTree));

    boost::optional<bool> value=readFrom.get_optional<bool>(DISABLE_NON_KEY_EVENT_KEY);
    if (value){
        m_disableNonKeyEvent=*value;
    }

    value=readFrom.get_optional<bool>(DISABLE_UNMAPPED_KEY_KEY);
    if(value){
        m_disableUnmappedKey=*value;
    }


    for(auto &value:readFrom.get_child(KEY_MAPS_KEY)){

        KeyMap thisOne;

        thisOne.fromKey=value.second.get<StringType>(FROM_KEY);
        thisOne.toKey=value.second.get<StringType>(TO_KEY);

        m_keyMaps.push_back(thisOne);
    }
}

void Profile::write(ptree& writeTo)const{

    writeTo.put(NAME_KEY,m_name);
    writeTo.put(DESCRIPTION_KEY,m_description);

    ptree matcherTree;

    DeviceMatcher::write(*m_matcher,matcherTree);

    writeTo.add_child(MATCHER_KEY,matcherTree);

    writeTo.put(DISABLE_NON_KEY_EVENT_KEY,m_disableNonKeyEvent);
    writeTo.put(DISABLE_UNMAPPED_KEY_KEY,m_disableUnmappedKey);

    ptree keyMapsRoot;

    for(auto &keyMap:m_keyMaps){
        ptree keyMapTree;
        keyMapTree.put(FROM_KEY,keyMap.fromKey);
        keyMapTree.put(TO_KEY,keyMap.toKey);

        keyMapsRoot.put_child("KeyMap",keyMapTree);
    }

    writeTo.add_child(KEY_MAPS_KEY,keyMapsRoot);
}

    
vector<Profile> Profile::readList(const ptree& readFrom){

    vector<Profile> ret;

    for(auto& subTree:readFrom){

        ret.push_back(Profile());
        ret.back().read(subTree.second);
    }

    return ret;
}

static const string PROFILE_KEY="PROFILE";
    
void Profile::writeList(const vector<Profile>& toWrite,ptree& tree){

    for(auto &profile:toWrite){

        ptree thisNode;

        profile.write(thisNode);

        tree.add_child(PROFILE_KEY,thisNode);

    }
}

    
void Profile::addKeyMap(KeyMap keyMap){

    m_keyMaps.push_back(keyMap);
}

    
void Profile::setMatcher(DeviceMatcher *token){
    m_matcher.reset(token);
}
