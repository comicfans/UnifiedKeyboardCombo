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
#include "Utility.hpp"

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

    while(!m_quit){

        if(m_profileChanged){
            log(INFO,"profile changed ,reconfig","");

            ScopeLock locker(m_profileMutex);
            configure();
        }

    
        vector<InputDevice*> failedOnes;
        for(auto &device:m_currentDevices){
            bool thisOk=device->processEvent();

            if (!thisOk){
                log(ERROR,device->name()," process event failed, remove it");
                failedOnes.push_back(device.get());
            }
        }

        if (failedOnes.empty()){
            continue;
        }

        int index=0;
        auto newEnd=m_currentDevices.end();
        for(InputDevice *failed:failedOnes){
            do{
                if(m_currentDevices[index].get()==failed){
                    newEnd=m_currentDevices.erase(m_currentDevices.begin()+index);
                    break;
                }
            }while(++index);
        }

        m_currentDevices.resize(newEnd-m_currentDevices.begin());
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
        
    for(auto &inputDevice: deviceList){
   
        for(auto &profile:m_profiles){

            //if match one profile ,just use it.
            if(profile.matcher().matchDevice(*inputDevice)){

                log(DEBUG,"profile match device:",profile.name().c_str(),inputDevice->name().c_str());

                bool configureOk=inputDevice->configure(profile.keyMaps(),
                        profile.disableNonKeyEvent(),
                        profile.disableUnmappedKey());


                if(configureOk){
                
                    log(DEBUG,inputDevice->name(),"configure ok");
                    m_currentDevices.push_back(std::move(inputDevice));
                }   else{

                    log(ERROR,inputDevice->name(),"configure failed");
                }
                break;

            }
        }
    }

    if (!m_currentDevices.empty()) {
        log(DEBUG,"matched devices list:","");
        for(auto &dev:m_currentDevices){
            log(DEBUG,dev->description(),"");
        }
    }
    

    m_profileChanged=false;
}

void MainLoop::start(){

    std::lock_guard<std::mutex> locker(m_startMutex);

    configure();

    enterLoop();

}

