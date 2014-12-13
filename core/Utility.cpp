#include "Utility.hpp"

#include <algorithm>
#include <iostream>
#include <boost/algorithm/string.hpp>

#if __linux
    #include <libevdev/libevdev.h>
    static auto& Cout=std::cout;
    static auto& Cin=std::cin;
#elif _WIN32    
    auto& Cout=std::wcout;
    auto& Cin=std::wcin;
#endif

void stripWildString(StringType& toStrip){

    auto end=std::unique(toStrip.begin(),toStrip.end(),
            [](StringType::value_type prev,StringType::value_type next)->bool{
            return (prev==MATCH_ALL_WILD[0]) && 
                   (next==MATCH_ALL_WILD[0]);
            });

    toStrip.resize(end-toStrip.begin());
}

bool simpleWildMatch(const StringType& toTest,const StringType & wild){

    StringType usedWild=wild;

    stripWildString(usedWild);

    if(usedWild.empty()){
        return true;
    }

    if (toTest.empty()){
        return usedWild==MATCH_ALL_WILD;
    }

    std::vector<StringType> tokens;

    boost::split(tokens,usedWild,boost::is_any_of(MATCH_ALL_WILD),
            boost::token_compress_on);

    auto newEnd=std::remove(tokens.begin(),tokens.end(),StringType(_T("")));

    tokens.resize(newEnd-tokens.begin());

    if(tokens.empty()){
        //only MATCH_ALL
        return true;
    }

    bool firstNotAny=(usedWild[0]!=MATCH_ALL_WILD[0]);
    bool lastNotAny=(usedWild.back()!=MATCH_ALL_WILD[0]);

    StringType::size_type searchPos=0;

    for(StringType::size_type i=0;i<tokens.size();++i){

        auto & thisWild=tokens[i];

        StringType::size_type foundPos=toTest.find(thisWild,searchPos);
        if(foundPos==StringType::npos){
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

static LogLevel ukc_logLevel=UKC_TRACE;

void setLogLevel(LogLevel level){
    ukc_logLevel=level;
}

static const StringType::value_type* const LOG_LEVEL_STRING[4]={
    _T("[UKC_TRACE]"),_T("[UKC_DEBUG]"),_T("[UKC_INFO]"),_T("[UKC_ERROR]")};

void ukc_log(LogLevel level,const StringType::value_type *message){

    if (level>=ukc_logLevel) {
        Cout<<message<<'\n';
    }
}

void ukc_log(LogLevel level,const StringType& message){

    if (level>=ukc_logLevel) {
        Cout<<message<<'\n';
    }
}

void ukc_log(LogLevel level,const StringType::value_type *const message,
        int arg0){

    if (level>=ukc_logLevel) {
        Cout<<LOG_LEVEL_STRING[level]<<message<<": "<<arg0<<'\n';
    }
}

void ukc_log(LogLevel level,const StringType::value_type *const message,int arg0,int arg1){

    if (level>=ukc_logLevel) {
        Cout<<LOG_LEVEL_STRING[level]<<message<<_T(": ")<<arg0<<_T(",")<<arg1<<'\n';
    }
}

void ukc_log(LogLevel level,const StringType::value_type *message,const StringType::value_type * arg0,const StringType::value_type * arg1){

    if (level>=ukc_logLevel) {
        Cout<<LOG_LEVEL_STRING[level]<<message<<_T(": ")<<arg0<<_T(",")<<arg1<<'\n';
    }
}

void ukc_log(LogLevel level,const StringType& message,
        const StringType::value_type* const arg0,
        const StringType::value_type* const arg1){

    if (level>=ukc_logLevel) {
        Cout<<LOG_LEVEL_STRING[level]<<message<<_T(": ")<<arg0<<_T(",")<<arg1<<'\n';
    }
}

#ifdef __WIN32
#elif defined __linux
void ukc_log(LogLevel level,const char *message ,input_event *evdev_event){
    if (level>=ukc_logLevel) {
        Cout<<LOG_LEVEL_STRING[level]<<message<<
            libevdev_event_type_get_name(evdev_event->type)
            <<" "<<libevdev_event_code_get_name(evdev_event->type,evdev_event->code)            <<" value :"<<evdev_event->value<<'\n';
    }
}
#endif
