#include "MainLoop.hpp"

#include <boost/property_tree/json_parser.hpp>

#include "PlatformInc.hpp"
#include "DeviceMatcher.hpp"
#include "Utility.hpp"

#include <sys/epoll.h>
#include <sys/signalfd.h>
#include <signal.h>

#include <sys/inotify.h>


void MainLoop::createInotifyFd()
{
  /* Create new inotify device */
  if ((m_inotifyFd = inotify_init1 (O_NONBLOCK)) < 0){
      ukc_log(ERROR,"can not create inotify",m_inotifyFd);
      return ;
  }

  if ((m_watch[WATCH_INPUT]=inotify_add_watch (
                  m_inotifyFd,EVENT_INPUT_PATH,IN_CREATE))< 0){
      ukc_log(ERROR,"can not create inotify for ",EVENT_INPUT_PATH);
  }

  //watch 
  if ((m_watch[WATCH_CONFIG]=inotify_add_watch (m_inotifyFd,
                  DEFAULT_CONFIG_JSON,IN_CLOSE_WRITE|IN_DELETE_SELF|IN_MODIFY|IN_MOVE_SELF))<0){

      ukc_log(ERROR,"can not create inotify watch for ",DEFAULT_CONFIG_JSON);
  }

  //test
  return; 

  auto pwd=get_current_dir_name();

  if((m_watch[WATCH_PWD]=inotify_add_watch(m_inotifyFd,pwd,IN_CREATE))<0){

      ukc_log(ERROR,"can not create inotify watch for ",pwd);
  }

  free(pwd);

}

static int
createSignalFd()
{
    int signal_fd=-1;
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
        ukc_log(ERROR,"can not create signal fd",errno);
        return -1;
    }

    return signal_fd;
}

void MainLoop::enterLoop(){

    m_epollFd=epoll_create1(0);

    if(m_epollFd==-1){
        ukc_log(ERROR,"create epoll fd failed","");
        return;
    }

        
    epoll_event ev;
    for(auto it=m_currentDevices.begin(),end=m_currentDevices.end();
            it!=end;){

        auto &device=it->second;

        if (!device->registerPoll(m_epollFd)){
            it=m_currentDevices.erase(it);
            continue;
        }
        
        ++it;
    }

    //hanlde signal in main loop , use signal to trigger reconfigure
    m_signalFd=createSignalFd();

    if (m_signalFd!=-1)    {
        ev.events=EPOLLIN;
        ev.data.fd=m_signalFd;
        if(epoll_ctl(m_epollFd,EPOLL_CTL_ADD,m_signalFd,&ev)==-1){
            ukc_log(ERROR,"can not add poll for signal",errno);
        }
    }
    

    createInotifyFd();

    if (m_inotifyFd!=-1 && (m_watch[0]!=-1 || m_watch[1]!=-1)) {

        //at least one watch is ok

        ev.events=EPOLLIN;
        ev.data.fd=m_inotifyFd;
        if(epoll_ctl(m_epollFd,EPOLL_CTL_ADD,m_inotifyFd,&ev)==-1){
            ukc_log(ERROR,"can not add poll for inotify","");
        }
    }


    while(!m_quit){

        epoll_event allPollEvents[m_currentDevices.size()+2];

        ukc_log(TRACE,"begin epoll on all input fds","");

        int rc=epoll_wait(m_epollFd,allPollEvents,m_currentDevices.size()+2,-1);

        ukc_log(TRACE,"epoll return ",rc);

        if (rc==-1){
            ukc_log(ERROR,"wait error:",errno);
            break;
        }

        for(int i=0;i<rc;++i){

            int fd=allPollEvents[i].data.fd;

            auto it=m_currentDevices.find(fd);
            if(it!=m_currentDevices.end()){
                InputDevice *inputDevice=it->second;

                ukc_log(TRACE,"process event of ",inputDevice->name().c_str());

                bool thisOk=inputDevice->processEvent();

                if (thisOk){

                    continue;
                }

                ukc_log(ERROR,inputDevice->name()," process event failed, remove it");

                m_currentDevices.erase(it);
                continue;
            }

            if(fd==m_signalFd){
               
                processSignal();
                continue;

                //process signal
            }

            if(fd==m_inotifyFd){

                processInotify();
                continue;
            }
            BOOST_ASSERT(false);
        }
    }


    close(m_epollFd);

    if (m_signalFd!=-1)    {
        close(m_signalFd);
    }

    if (m_inotifyFd!=-1){
        close(m_inotifyFd);
    }

    m_currentDevices.clear();
}


void MainLoop::reloadConfig(){

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


bool MainLoop::configureAddIfMatch(unique_ptr<InputDevice> inputDevice){

    for(auto &profile:m_profiles){

            //if match one profile ,just use it.
            if(profile.matcher().matchDevice(*inputDevice)){

                ukc_log(DEBUG,"profile match device:",profile.name().c_str(),inputDevice->name().c_str());

                bool configureOk=inputDevice->configure(profile.keyMaps(),
                        profile.disableNonKeyEvent(),
                        profile.disableUnmappedKey());


                if(configureOk){
                
                    ukc_log(DEBUG,inputDevice->name(),"configure ok");
                    auto toInsert=inputDevice.release();
                    int fd=toInsert->evdevFd();
                    m_currentDevices.insert(fd,toInsert);
                    return true;
                }   else{

                    ukc_log(ERROR,inputDevice->name(),"configure failed");
                    return false;
                }
                break;

            }
        }

    return false;
}

void MainLoop::configureAll(){

    //build match roles
    //

    m_currentDevices.clear();

    auto deviceList= InputDevice::scanDevices();

    //match device 
    //
        
    for(auto &inputDevice: deviceList){
        
        configureAddIfMatch(std::move(inputDevice));
    }

    if (!m_currentDevices.empty()) {
        ukc_log(DEBUG,"matched devices list:","");
        for(const auto &dev:m_currentDevices){
            ukc_log(DEBUG,dev.second->description(),"");
        }
    }
    

}

void MainLoop::start(){

    reloadConfig();

    configureAll();

    enterLoop();
}


void MainLoop::processSignal(){

    ukc_log(INFO,"receiving signal","");

    bool firstRead=true;
    bool ok=false;
    while(true){

        signalfd_siginfo fdsi;

        int readNumber=read (m_signalFd,&fdsi,sizeof (fdsi));
        if(firstRead && (readNumber!=sizeof(fdsi))){
            ukc_log(ERROR,"read signal failed","");
            m_quit=true;
            return;
        }

        firstRead=false;

        if(readNumber==0){
            return;
        }

        if(fdsi.ssi_signo == SIGINT ||
                fdsi.ssi_signo == SIGTERM){

    
            ukc_log(INFO,"receiving SIGINT or SIGTERM","quit");
            m_quit=true;
            return;
        }
        
        //TODO other signal ?
        ukc_log(DEBUG,"other signal :",fdsi.ssi_signo);
    }
}

#define IN_EVENT_DATA_LEN (sizeof(struct inotify_event))
#define IN_EVENT_NEXT(event, length)            \
  ((length) -= (event)->len,                    \
   (struct inotify_event*)(((char *)(event)) +	\
                           (event)->len))
#define IN_EVENT_OK(event, length)                  \
  ((long)(length) >= (long)IN_EVENT_DATA_LEN &&	    \
   (long)(event)->len >= (long)IN_EVENT_DATA_LEN && \
   (long)(event)->len <= (long)(length))


	   
static constexpr int MAX_INOTIFY_SIZE = sizeof(struct inotify_event) + NAME_MAX + 1;

void MainLoop::processInotify(){

    ukc_log(INFO,"inotify received","");

    char buffer[MAX_INOTIFY_SIZE*10];

    int readLength=read (m_inotifyFd,
            buffer,
            MAX_INOTIFY_SIZE);

    if (readLength<=0){

        ukc_log(ERROR,"read inotify failed","");
        return;
    }


    inotify_event *event = (inotify_event *)buffer;


    while (IN_EVENT_OK (event, readLength))
    {

        // create

        ukc_log(INFO,"new device plugined",event->name);

        if (event->wd==m_watch[WATCH_INPUT] && 
            (strncmp(event->name,"event",5)==0)){
            //new input device created
            BOOST_ASSERT(event->mask & IN_CREATE);

            ukc_log(DEBUG,"new evdev node found ",event->name);

            InputDevice *inputDevice=InputDevice::tryCreateNew((string(EVENT_INPUT_PATH)+event->name).c_str());

            if (inputDevice) {
                ukc_log(DEBUG,"new input device created for ",event->name);
                if(configureAddIfMatch(unique_ptr<InputDevice>(inputDevice))){

                    ukc_log(INFO,"add new device to poll",inputDevice->name().c_str());
                    if (!inputDevice->registerPoll(m_epollFd)) {
                        m_currentDevices.erase(inputDevice->evdevFd());
                    }
                }
            }

        }else if(event->wd==m_watch[WATCH_CONFIG]){
                
            ukc_log(DEBUG,"config file changed reconfig","");
            reloadConfig();
            configureAll();
        }

        event = IN_EVENT_NEXT (event, readLength);
    }
}

