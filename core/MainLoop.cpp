/*
 * =====================================================================================
 *
 *       Filename:  MainLoop.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/28/2014 04:17:52 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), |wangxinyu@chison.com.cn|
 *   Organization:  
 *
 * =====================================================================================
 */

#include "MainLoop.hpp"

#include <boost/property_tree/json_parser.hpp>

#include "PlatformInc.hpp"
#include "DeviceMatcher.hpp"

using std::lock_guard;

typedef lock_guard<mutex> ScopeLock;

void MainLoop::addProfile(const Profile& profile){


    {

        bool started=!m_startMutex.try_lock();

        //not started
        if(!started){
            m_profiles.push_back(profile);
            m_startMutex.unlock();
            return;
        }
    }

    // already started 
    //

    ScopeLock locker(m_profileMutex);

    m_profileChanged=true;

    m_profiles.push_back(profile);

}

void MainLoop::enterLoop(){

    vector<InputDevice*> failedOnes;
    while(!m_quit){

        if(m_profileChanged){

            ScopeLock locker(m_profileMutex);
            configure();
        }

        for(auto &device:m_currentDevices){
            bool thisOk=device->processEvent();

            if (!thisOk){
                failedOnes.push_back(device.get());
            }
        }

        if (failedOnes.empty()){
            continue;
        }

        int index=0;
        for(InputDevice *failed:failedOnes){
            do{
                if(m_currentDevices[index].get()==failed){
                    m_currentDevices.erase(m_currentDevices.begin()+index);
                    break;
                }
            }while(++index);
        }
    }

    m_currentDevices.clear();

}

static const string DEFAULT_CONFIG_JSON="ukc.json";

MainLoop::MainLoop(){

    //reade default config

    ptree readTree;

    try{
    
        boost::property_tree::read_json(DEFAULT_CONFIG_JSON,readTree);

        m_profiles=Profile::readList(readTree);

    }catch(boost::property_tree::json_parser_error &t){

        //do nothing
    }

}

void MainLoop::configure(){

    //build match roles
    //

    m_currentDevices.clear();

    auto deviceList= InputDevice::scanDevices();

    //match device 
    //
    for(auto &profile:m_profiles){

        for(auto &inputDevice: deviceList){

            if(profile.matcher().matchDevice(*inputDevice)){
                inputDevice->setKeyMaps(profile.keyMaps());

                bool prepare=inputDevice->prepare();

                if(!prepare){
                    continue;
                }

                m_currentDevices.push_back(std::move(inputDevice));
            }
        }
    }

    m_profileChanged=false;
}

void MainLoop::start(){

    std::lock_guard<std::mutex> locker(m_startMutex);

    configure();

    enterLoop();

}

