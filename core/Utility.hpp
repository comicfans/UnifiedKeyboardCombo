
#ifndef UTILITY_HPP_GQB8XJXS
#define UTILITY_HPP_GQB8XJXS

#include "StringType.hpp"


struct input_event;

constexpr const StringType::value_type* MATCH_ALL_WILD=_T("*");

void stripWildString(StringType& toStrip);

bool simpleWildMatch(const StringType& toTest,const StringType& wild);

enum LogLevel{TRACE,DEBUG,INFO,ERROR};

void setLogLevel(LogLevel level);

void ukc_log(LogLevel level,const char *message);

void ukc_log(LogLevel level,const StringType& message);

void ukc_log(LogLevel level,const char *message,int arg0,int arg1);

void ukc_log(LogLevel level,const char *message,int arg0);

void ukc_log(LogLevel level,const char *message,const char* arg0="",const char* arg1="");

void ukc_log(LogLevel level,const StringType& message,const char *arg0="",const char* arg1="");

#ifdef __WIN32
#elif defined __linux
void ukc_log(LogLevel level,const char *message ,input_event *evdev_event);
#endif

#endif /* end of include guard: UTILITY_HPP_GQB8XJXS */

