
#ifndef UTILITY_HPP_GQB8XJXS
#define UTILITY_HPP_GQB8XJXS

#include <string>

using std::string;

struct input_event;

constexpr const char * MATCH_ALL_WILD="*";

void stripWildString(string& toStrip);

bool simpleWildMatch(const string& toTest,const string & wild);

enum LogLevel{TRACE,DEBUG,INFO,ERROR};

void setLogLevel(LogLevel level);

void ukc_log(LogLevel level,const char *message);

void ukc_log(LogLevel level,const string& message);

void ukc_log(LogLevel level,const char *message,int arg0,int arg1);

void ukc_log(LogLevel level,const char *message,int arg0);

void ukc_log(LogLevel level,const char *message,const char* arg0="",const char* arg1="");

void ukc_log(LogLevel level,const string& message,const char *arg0="",const char* arg1="");

void ukc_log(LogLevel level,const char *message ,input_event *evdev_event);

#endif /* end of include guard: UTILITY_HPP_GQB8XJXS */

