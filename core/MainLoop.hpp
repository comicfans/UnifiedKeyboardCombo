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

#include <memory>
#include <boost/ptr_container/ptr_unordered_map.hpp>


                
struct signalfd_siginfo;

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

    bool m_quit=false;

    MainLoop();

    ~MainLoop();

    void enterLoop();

    vector<Profile> m_profiles;

    boost::ptr_unordered_map<int,InputDevice> m_currentDevices;

    void processSignals(const signalfd_siginfo& sigInfo);

    void processInotify();

};


#endif /* end of include guard: MAINLOOP_HPP_R3LQMSEA */

