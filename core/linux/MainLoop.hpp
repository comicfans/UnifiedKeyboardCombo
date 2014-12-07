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


                
struct inotify_event;

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

    bool configureAddIfMatch(unique_ptr<InputDevice> one);

    void configureAll();

    void reloadConfig(bool createWatch);

    void pollAll();

    void processConfigChange(const inotify_event* event);

    void createWatchConfig();

    void createWatchCwd();

    bool m_quit=false;

    MainLoop()=default;

    void enterLoop();

    vector<Profile> m_profiles;

    boost::ptr_unordered_map<int,InputDevice> m_currentDevices;

    void createInotifyFd();

    void processSignal();

    void processInotify();

    int m_epollFd=-1;

    int m_inotifyFd=-1;

    int m_signalFd=-1;

    //config/cwd only one will be active
    enum WatchIndex{WATCH_INPUT,WATCH_CONFIG,WATCH_CWD};
    int m_watch[3]={-1,-1,-1};

};


#endif /* end of include guard: MAINLOOP_HPP_R3LQMSEA */

