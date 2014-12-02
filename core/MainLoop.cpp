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

    int epollFd=epoll_create1(0);

    if(epollFd==-1){
        ukc_log(ERROR,"create epoll fd failed","");
        return;
    }

    for(auto it=m_currentDevices.begin(),end=m_currentDevices.end();
            it!=end;){

        auto &device=*it;

        epoll_event ev;
        ev.events=EPOLLIN;
        ev.data.ptr=&device;

        if(epoll_ctl(epollFd,EPOLL_CTL_ADD,device.evdevFd(),&ev)==-1){
            ukc_log(ERROR,"can not mod poll ev for ",device.name().c_str());
            it=m_currentDevices.erase(it);
            continue;
        }
        ++it;
    }

    epoll_event allPollEvents[m_currentDevices.size()];

    while(true){


        ukc_log(TRACE,"begin epoll on all input fds","");

        int rc=epoll_wait(epollFd,allPollEvents,m_currentDevices.size(),-1);

        ukc_log(TRACE,"epoll return ",rc);

        if (rc==-1){
            ukc_log(ERROR,"wait error:",errno);
            break;
        }

        for(int i=0;i<rc;++i){

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

            m_currentDevices.erase(m_currentDevices.find(*inputDevice));
        }

        if (m_currentDevices.empty()){
            break;
        }

    }

    for(auto & dev:m_currentDevices){
        epoll_ctl(epollFd,EPOLL_CTL_DEL,dev.evdevFd(),NULL);
    }

    close(epollFd);

    m_currentDevices.clear();

}

static const string DEFAULT_CONFIG_JSON="ukc.json";

static int
createInotifyFd()
{
  int i;
  int inotify_fd;

  /* Create new inotify device */
  if ((inotify_fd = inotify_init ()) < 0)
    {
      fprintf (stderr,
               "Couldn't setup new inotify device: '%s'\n",
               strerror (errno));
      return -1;
    }

  /* Allocate array of monitor setups */
  n_monitors = argc - 1;
  monitors = malloc (n_monitors * sizeof (monitored_t));

  /* Loop all input directories, setting up watches */
  for (i = 0; i < n_monitors; ++i)
    {
      monitors[i].path = strdup (argv[i + 1]);
      if ((monitors[i].wd = inotify_add_watch (inotify_fd,
                                               monitors[i].path,
                                               event_mask)) < 0)
        {
          fprintf (stderr,
                   "Couldn't add monitor in directory '%s': '%s'\n",
                   monitors[i].path,
                   strerror (errno));
          exit (EXIT_FAILURE);
        }
      printf ("Started monitoring directory '%s'...\n",
              monitors[i].path);
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

  if (sigprocmask (SIG_BLOCK, &sigmask, NULL) < 0)
    {
      fprintf (stderr,
               "Couldn't block signals: '%s'\n",
               strerror (errno));
      return -1;
    }

  /* Get new FD to read signals from it */
  if ((signal_fd = signalfd (-1, &sigmask, 0)) < 0)
    {
      fprintf (stderr,
               "Couldn't setup signal FD: '%s'\n",
               strerror (errno));
      return -1;
    }

  return signal_fd;
}
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
        for(auto &dev:m_currentDevices){
            ukc_log(DEBUG,dev.description(),"");
        }
    }
    

    m_profileChanged=false;
}

void MainLoop::start(){

    std::lock_guard<std::mutex> locker(m_startMutex);

    configure();

    if (m_currentDevices.empty()){
        //TODO use pipe poll to do main loop
        ukc_log(ERROR,"no matching devices","");
        return;
    }

    enterLoop();

}

        
std::size_t MainLoop::Hasher::operator()(const InputDevice& value)const{
    boost::hash<const void*> hasher;
    return hasher(&value);
}

        
bool MainLoop::Equaler::operator()(const InputDevice &lhs,const InputDevice &rhs)const{
    return &lhs==&rhs;
}

