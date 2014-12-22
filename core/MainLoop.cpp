#include "MainLoop.hpp"

#include <boost/property_tree/json_parser.hpp>

#include "PlatformInc.hpp"
#include "DeviceMatcher.hpp"
#include "Utility.hpp"

#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <sys/signalfd.h>
#include <signal.h>

#include <sys/inotify.h>


void MainLoop::createInotifyFd(){
  /* Create new inotify device */
  if ((m_inotifyFd = inotify_init1 (O_NONBLOCK)) < 0){
      ukc_log(ERROR,"can not create inotify",m_inotifyFd);
      return ;
  }

  if ((m_watch[WATCH_INPUT]=inotify_add_watch (
                  m_inotifyFd,EVENT_INPUT_PATH,IN_CREATE))< 0){
      ukc_log(ERROR,"can not create inotify watch on ",
              EVENT_INPUT_PATH);

  }

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

void MainLoop::pollAll(){
    for(auto it=m_currentDevices.begin(),end=m_currentDevices.end();
            it!=end;){

        auto &device=it->second;

        if (!device->registerPoll(m_epollFd)){
            it=m_currentDevices.erase(it);
            continue;
        }
        
        ++it;
    }

}

void MainLoop::enterLoop(){

    m_epollFd=epoll_create1(0);

    if(m_epollFd==-1){
        ukc_log(ERROR,"create epoll fd failed","");
        return;
    }

    

    pollAll();
        
    //hanlde signal in main loop , use signal to trigger reconfigure
    m_signalFd=createSignalFd();

    epoll_event ev;
    if (m_signalFd!=-1)    {
        ev.events=EPOLLIN;
        ev.data.fd=m_signalFd;
        if(epoll_ctl(m_epollFd,EPOLL_CTL_ADD,m_signalFd,&ev)==-1){
            ukc_log(ERROR,"can not add poll for signal",errno);
        }
    }
    
    //delay inotify watch here to prevent too long 
    //device scan leads many config change events
    createInotifyFd();

    createWatchConfig();

    if (m_inotifyFd!=-1 && 
            (m_watch[0]!=-1 || m_watch[1]!=-1 
             || m_watch[2]!=-1)) {

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


void MainLoop::reloadConfig(bool createWatch){

    m_profiles.clear();

    //reade default config

    ptree readTree;

    try{
        ukc_log(INFO,"try reading config file",DEFAULT_CONFIG_JSON);
    
        boost::property_tree::read_json(DEFAULT_CONFIG_JSON,readTree);

        m_profiles=Profile::readList(readTree);

        if (createWatch) {
            createWatchConfig();
        }

        ukc_log(INFO,"read config profiles number :",m_profiles.size());

    }catch(boost::property_tree::json_parser_error &t){

        ukc_log(ERROR,t.message().c_str(),t.line());

        if(createWatch){
            createWatchCwd();
        }
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


    reloadConfig(false);

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


static constexpr int INOTIFY_EVENT_SIZE= sizeof(inotify_event);

void MainLoop::processInotify(){

    ukc_log(INFO,"inotify received","");

    unsigned int avail;
    int rc=ioctl(m_inotifyFd, FIONREAD, &avail);
    if(rc!=0){
        ukc_log(ERROR,"erro when getting inotify buffersize",strerror(errno)); 
    }

    char buffer[avail];
    const int readLength=read(m_inotifyFd, buffer, avail);

    
    if (readLength<=0){

        ukc_log(ERROR,"read inotify failed","");
        return;
    }

    BOOST_ASSERT(readLength==avail);

    inotify_event *event = (inotify_event *)buffer;

    bool atLeastOne=false; 
    //at least one
    while (true)
    {
         
        char* pTest=reinterpret_cast<char*>(event); 

        BOOST_ASSERT(pTest<buffer+avail); 

        if (event->wd==m_watch[WATCH_INPUT]){
            if(strncmp(event->name,"event",5)==0){
                //new input device created

                atLeastOne=true; 
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

            }else{
                atLeastOne=true; 
                ukc_log(DEBUG,"new device created, but not evdev based, ignored",event->name); 
            }
        }else {
                
            atLeastOne=true; 
            processConfigChange(event);
            
        }


        int thisSize=INOTIFY_EVENT_SIZE+event->len;

        event=(inotify_event*)((char*)event+thisSize);

        int leftBytes=buffer+readLength-(char*)event;

        if(leftBytes<INOTIFY_EVENT_SIZE){
            break;
        }

        if(leftBytes<INOTIFY_EVENT_SIZE+event->len){
            break;
        }

        ukc_log(TRACE,"left bytes :",leftBytes); 
    }

    BOOST_ASSERT(atLeastOne); 

}

void MainLoop::processConfigChange(const inotify_event * event){
    ukc_log(DEBUG,"file change id",event->wd); 
    if(event->wd==m_watch[WATCH_CONFIG]){
        ukc_log(DEBUG,"config file changed reconfig","");
        if (event->mask & IN_ATTRIB) {
            reloadConfig(false);
            configureAll();
            pollAll();
            return; 
        }
        

        if ((event->mask & IN_MOVE_SELF) || (event->mask & IN_DELETE_SELF)){    
            ukc_log(DEBUG,DEFAULT_CONFIG_JSON,"moved or deleted ");

            int rc=inotify_rm_watch(m_inotifyFd,m_watch[WATCH_CONFIG]);
            if (rc==-1){
                ukc_log(ERROR,"remove watch for config file failed ",strerror(errno));
            }
            m_watch[WATCH_CONFIG]=-1;    

            createWatchCwd(); 

            return;     
        }

    }

    if (event->wd==m_watch[WATCH_CWD]){
            
        ukc_log(TRACE,"file change in pwd",event->name);

        if(strcmp(event->name,DEFAULT_CONFIG_JSON)==0){

            ukc_log(INFO,DEFAULT_CONFIG_JSON,"found in pwd");

            //file created, or moved in
            //first remove self watch
            //
            int rc=inotify_rm_watch(m_inotifyFd,m_watch[WATCH_CWD]);
            if (rc==-1){
                ukc_log(ERROR,"remove watch for cwd failed ",strerror(errno));
            }

            m_watch[WATCH_CWD]=-1;

            //reload config will trigger add watch config
            // if it falied, 
            reloadConfig(true);
            configureAll();
            pollAll();
        }

        return; 
    }

}

void MainLoop::createWatchConfig(){

    ukc_log(DEBUG,"create watch on", DEFAULT_CONFIG_JSON);

    BOOST_ASSERT(m_watch[WATCH_CONFIG]==-1); 

    if((m_watch[WATCH_CONFIG]=inotify_add_watch(m_inotifyFd,
                    DEFAULT_CONFIG_JSON,
                    IN_CLOSE_WRITE|IN_MODIFY|IN_DELETE_SELF
                    |IN_MOVE_SELF|IN_ATTRIB))<0){
        ukc_log(ERROR,"can not create inotify for config ,instead create watch pwd","");

        createWatchCwd();
        return; 
    }

    ukc_log(DEBUG,"create config watch id",m_watch[WATCH_CONFIG]); 

}

void MainLoop::createWatchCwd(){

    auto pwd=get_current_dir_name();

    ukc_log(DEBUG,"create watch on ",pwd);

    BOOST_ASSERT(m_watch[WATCH_CWD]==-1); 

    if((m_watch[WATCH_CWD]=inotify_add_watch(m_inotifyFd,pwd,
                    IN_CREATE|IN_MOVED_TO))<0){

        ukc_log(ERROR,"can not create inotify watch for ",pwd);
    }

    ukc_log(DEBUG,"cwd watch id",m_watch[WATCH_CWD]); 

    free(pwd);
}

