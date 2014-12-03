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

#include <sys/epoll.h>
#include <sys/signalfd.h>
#include <signal.h>

#include <sys/inotify.h>

static int
createInotifyFd()
{
  int i;
  int inotify_fd;

  /* Create new inotify device */
  if ((inotify_fd = inotify_init1 (O_NONBLOCK)) < 0){
      ukc_log(ERROR,"can not create inotify",inotify_fd);
      return -1;
  }

  if (inotify_add_watch (inotify_fd,EVENT_INPUT_PATH,IN_CREATE) < 0){
      ukc_log(ERROR,"can not create inotify",inotify_fd);
      return -1;
  }

  return inotify_fd;
}

static int
createSignalFd()
{
    int signal_fd;
    sigset_t sigmask;

    /* We want to handle SIGINT and SIGTERM in the signal_fd, so we block them. */
    sigemptyset (&sigmask);
    sigaddset (&sigmask, SIGINT);
    sigaddset (&sigmask, SIGTERM);

    if (sigprocmask (SIG_BLOCK, &sigmask, NULL) < 0){

        ukc_log(ERROR,"can not block signals ","");
        return -1;
    }

    /* Get new FD to read signals from it */
    if ((signal_fd = signalfd (-1, &sigmask, 0)) < 0)
    {
        ukc_log(ERROR,"can not create signal fd","");
        return -1;
    }

    return signal_fd;
}

void MainLoop::enterLoop(){

    int epollFd=epoll_create1(0);

    if(epollFd==-1){
        ukc_log(ERROR,"create epoll fd failed","");
        return;
    }

        
    epoll_event ev;
    for(auto it=m_currentDevices.begin(),end=m_currentDevices.end();
            it!=end;){

        auto &device=it->second;

        ev.events=EPOLLIN;
        ev.data.fd=device->evdevFd();

        if(epoll_ctl(epollFd,EPOLL_CTL_ADD,device->evdevFd(),&ev)==-1){
            ukc_log(ERROR,"can not add poll ev for ",device->name().c_str());
            it=m_currentDevices.erase(it);
            continue;
        }
        ++it;
    }

    //hanlde signal in main loop , use signal to trigger reconfigure
    int signalFd=createSignalFd();

    ev.events=EPOLLIN;
    ev.data.fd=signalFd;
    if(epoll_ctl(epollFd,EPOLL_CTL_ADD,signalFd,&ev)==-1){
        ukc_log(ERROR,"can not add poll for signal","");
    }

    int inotifyFd=createInotifyFd();

    ev.events=EPOLLIN;
    ev.data.fd=signalFd;
    if(epoll_ctl(epollFd,EPOLL_CTL_ADD,signalFd,&ev)==-1){
        ukc_log(ERROR,"can not add poll for signal","");
    }

    epoll_event allPollEvents[m_currentDevices.size()+2];

    while(true){


        ukc_log(TRACE,"begin epoll on all input fds","");

        int rc=epoll_wait(epollFd,allPollEvents,m_currentDevices.size(),-1);

        ukc_log(TRACE,"epoll return ",rc);

        if (rc==-1){
            ukc_log(ERROR,"wait error:",errno);
            break;
        }

        for(int i=0;i<rc;++i){

            int fd=allPollEvents[i].data.fd;

            auto it=m_currentDevices.find(fd);
            if(it!=m_currentDevices.end()){
                InputDevice *inputDevice=reinterpret_cast<InputDevice*>(
                        allPollEvents[i].data.ptr);

                ukc_log(TRACE,"process event of ",inputDevice->name().c_str());

                bool thisOk=inputDevice->processEvent();

                if (thisOk){

                    continue;
                }

                ukc_log(ERROR,inputDevice->name()," process event failed, remove it");

                //last argument should not be NULL with kernel 2.6.9
                //I think this can be ignored
                epoll_ctl(epollFd,EPOLL_CTL_DEL,inputDevice->evdevFd(),NULL);

                m_currentDevices.erase(it);
            }

            if(fd==signalFd){
               

                //process signal
            }else if(fd==inotifyFd){
                //process inotify

            }
            
        }

        if (m_currentDevices.empty()){
            break;
        }

    }


    close(epollFd);

    if (signalFd!=-1)    {
        close(signalFd);
    }

    if(inotifyFd!=-1){
        close(inotifyFd);
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

        ukc_log(INFO,"read config profiles number :",m_profiles.size());

    }catch(boost::property_tree::json_parser_error &t){

        ukc_log(ERROR,t.message().c_str(),t.line());
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

                ukc_log(DEBUG,"profile match device:",profile.name().c_str(),inputDevice->name().c_str());

                bool configureOk=inputDevice->configure(profile.keyMaps(),
                        profile.disableNonKeyEvent(),
                        profile.disableUnmappedKey());


                if(configureOk){
                
                    ukc_log(DEBUG,inputDevice->name(),"configure ok");
                    m_currentDevices.insert(inputDevice.release());
                }   else{

                    ukc_log(ERROR,inputDevice->name(),"configure failed");
                }
                break;

            }
        }
    }

    if (!m_currentDevices.empty()) {
        ukc_log(DEBUG,"matched devices list:","");
        for(const auto &dev:m_currentDevices){
            ukc_log(DEBUG,dev.second->description(),"");
        }
    }
    

}

void MainLoop::start(){

    configure();

    if (m_currentDevices.empty()){
        //TODO use pipe poll to do main loop
        ukc_log(ERROR,"no matching devices","");
        return;
    }

    enterLoop();

}


void MainLoop::processSignal(){
 struct signalfd_siginfo fdsi;

                bool firstRead=true;
                bool ok=false;
                while(true){

                    int readNumber=read (signalFd,&fdsi,sizeof (fdsi));
                    if(firstRead && (readNumber!=sizeof(fdsi))){
                        break;
                    }
                    firstRead=false;

                    if(readNumber==0){
                        ok=true;
                        break;
                    }
                }

                if(!ok){

                    //signal process failed
                }
}
