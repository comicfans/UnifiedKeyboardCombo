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

#include <boost/property_tree/ptree_fwd.hpp>

#include "KeyMap.hpp"

using std::string;
using std::vector;
using std::unique_ptr;

using boost::property_tree::ptree;

class DeviceMatcher;
class Profile
{
public:

    Profile();

    Profile(const Profile& rhs);

    Profile& operator==(const Profile& rhs);

    const string& name()const{return m_name;}

    void setName(const string& nameSet){m_name=nameSet;};

    const string& description()const{return m_description;}

    const DeviceMatcher &matcher(){return *m_matcher;}

    void setDescription(const string& descriptionSet){
        m_description=descriptionSet;}

    void setMatcher(DeviceMatcher *token);

    void addKeyMap(KeyMap keyMap);

    void read(const ptree& readFrom);

    void write(ptree& writeTo)const;

    static vector<Profile> readList(const ptree& readFrom);

    static void writeList(const vector<Profile>& toWrite,ptree& tree);

    const vector<KeyMap> & keyMaps()const{return m_keyMaps;}

private:

    string m_name;

    string m_description;

    unique_ptr<DeviceMatcher> m_matcher;

    vector<KeyMap> m_keyMaps;
    

};


#endif /* end of include guard: PROFILE_HPP_TZBYL2JP */

