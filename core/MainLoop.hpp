/*
 * =====================================================================================
 *
 *       Filename:  MainLoop.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/28/2014 04:18:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef MAINLOOP_HPP_R3LQMSEA
#define MAINLOOP_HPP_R3LQMSEA

#include "Profile.hpp"

#include "PlatformDef.hpp"

#include <atomic>
#include <mutex>
#include <memory>
#include <unordered_map>


using std::mutex;
using std::unique_ptr;

using std::unordered_map;

class MainLoop
{
public:

    static MainLoop& instance(){
        static MainLoop s_instance;
        return s_instance;
    }

    const vector<Profile> &profiles(){return m_profiles;}

    void start();

    void addProfile(const Profile& profile);

    void clearProfiles();

private:

    void configure();

    mutex m_startMutex;

    mutex m_profileMutex;

    bool m_profileChanged=true;

    bool m_quit=false;

    MainLoop();

    void enterLoop();

    vector<Profile> m_profiles;

    struct Deleter{
        void operator()(InputDevice *p);
    };

    vector<unique_ptr<InputDevice>> m_currentDevices;
};


#endif /* end of include guard: MAINLOOP_HPP_R3LQMSEA */

