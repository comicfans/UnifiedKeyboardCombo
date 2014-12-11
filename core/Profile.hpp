/*
 * =====================================================================================
 *
 *       Filename:  Profile.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/28/2014 04:20:03 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef PROFILE_HPP_TZBYL2JP
#define PROFILE_HPP_TZBYL2JP

#include <memory>
#include <vector>
#include <string>


#include "KeyMap.hpp"
#include "TreeType.hpp"

using std::vector;
using std::unique_ptr;


static constexpr const StringType::value_type * const DEFAULT_CONFIG_JSON=_T("ukc.json");

class DeviceMatcher;
class Profile
{
public:

    Profile();

    Profile(const Profile& rhs);

    Profile& operator=(const Profile& rhs);

    const StringType& name()const{return m_name;}

    void setName(const StringType& nameSet){m_name=nameSet;};

    const StringType& description()const{return m_description;}

    const DeviceMatcher &matcher(){return *m_matcher;}

    void setDescription(const StringType& descriptionSet){
        m_description=descriptionSet;}

    void setMatcher(DeviceMatcher *token);

    void addKeyMap(KeyMap keyMap);

    void read(const TreeType& readFrom);

    void write(TreeType& writeTo)const;

    static vector<Profile> readList(const TreeType& readFrom);

    static void writeList(const vector<Profile>& toWrite,TreeType& tree);

    const vector<KeyMap> & keyMaps()const{return m_keyMaps;}

    void setDisableNonKeyEvent(bool value){m_disableNonKeyEvent=value;}

    bool disableNonKeyEvent()const{return m_disableNonKeyEvent;}

    void setDisableUnmappedKey(bool value){m_disableUnmappedKey=value;}

    bool disableUnmappedKey()const{return m_disableUnmappedKey;}

private:

    StringType m_name;

    StringType m_description;

    unique_ptr<DeviceMatcher> m_matcher;

    vector<KeyMap> m_keyMaps;
    
    bool m_disableNonKeyEvent=true;

    bool m_disableUnmappedKey=false;

};


#endif /* end of include guard: PROFILE_HPP_TZBYL2JP */

