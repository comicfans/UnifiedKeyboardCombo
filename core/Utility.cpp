#include "Utility.hpp"

#include <algorithm>
#include <iostream>
#include <boost/algorithm/string.hpp>

#if __linux
    #include <libevdev/libevdev.h>
#endif

void stripWildString(string& toStrip){

    auto end=std::unique(toStrip.begin(),toStrip.end(),
            [](char prev,char next)->bool{
            return (prev==MATCH_ALL_WILD[0]) && 
                   (next==MATCH_ALL_WILD[0]);
            });

    toStrip.resize(end-toStrip.begin());
}

bool simpleWildMatch(const string& toTest,const string & wild){

    string usedWild=wild;

    stripWildString(usedWild);

    if(usedWild.empty()){
        return true;
    }

    if (toTest.empty()){
        return usedWild==MATCH_ALL_WILD;
    }

    std::vector<string> tokens;

    boost::split(tokens,usedWild,boost::is_any_of(MATCH_ALL_WILD),
            boost::token_compress_on);

    auto newEnd=std::remove(tokens.begin(),tokens.end(),string(""));

    tokens.resize(newEnd-tokens.begin());

    if(tokens.empty()){
        //only MATCH_ALL
        return true;
    }

    bool firstNotAny=(usedWild[0]!=MATCH_ALL_WILD[0]);
    bool lastNotAny=(usedWild.back()!=MATCH_ALL_WILD[0]);

    int searchPos=0;

    for(int i=0;i<tokens.size();++i){

        auto & thisWild=tokens[i];

        int foundPos=toTest.find(thisWild,searchPos);
        if(foundPos==string::npos){
            return false;
        }

        if(i==0 && firstNotAny && foundPos!=0){
            //head must match
            return false;
        }

        searchPos=foundPos+thisWild.length();

        if(searchPos==toTest.length()){

            if (i!=tokens.size()-1) {
                //toTest not long enough
                return false;
            }
        }
    }

    //all token cosumed ,input not cosumed
    if(searchPos!=toTest.length() && lastNotAny){

        return false;
    }

    return true;
}

static LogLevel ukc_logLevel=TRACE;

void setLogLevel(LogLevel level){
    ukc_logLevel=level;
}

static const char * const LOG_LEVEL_STRING[4]={"[TRACE]","[DEBUG]","[INFO]","[ERROR]"};

void ukc_log(LogLevel level,const char *message){

    if (level>=ukc_logLevel) {
        std::cout<<message<<'\n';
    }
}

void ukc_log(LogLevel level,const string& message){

    if (level>=ukc_logLevel) {
        std::cout<<message<<'\n';
    }
}

void ukc_log(LogLevel level,const char *message,int arg0){

    if (level>=ukc_logLevel) {
        std::cout<<LOG_LEVEL_STRING[level]<<message<<": "<<arg0<<'\n';
    }
}

void ukc_log(LogLevel level,const char *message,int arg0,int arg1){

    if (level>=ukc_logLevel) {
        std::cout<<LOG_LEVEL_STRING[level]<<message<<": "<<arg0<<","<<arg1<<'\n';
    }
}

void ukc_log(LogLevel level,const char *message,const char* arg0,const char* arg1){

    if (level>=ukc_logLevel) {
        std::cout<<LOG_LEVEL_STRING[level]<<message<<": "<<arg0<<","<<arg1<<'\n';
    }
}

void ukc_log(LogLevel level,const string& message,const char* arg0,const char* arg1){

    if (level>=ukc_logLevel) {
        std::cout<<LOG_LEVEL_STRING[level]<<message<<": "<<arg0<<","<<arg1<<'\n';
    }
}

void ukc_log(LogLevel level,const char *message ,input_event *evdev_event){
    if (level>=ukc_logLevel) {
        std::cout<<LOG_LEVEL_STRING[level]<<message<<
            libevdev_event_type_get_name(evdev_event->type)
            <<" "<<libevdev_event_code_get_name(evdev_event->type,evdev_event->code)            <<" value :"<<evdev_event->value<<'\n';
    }
}
