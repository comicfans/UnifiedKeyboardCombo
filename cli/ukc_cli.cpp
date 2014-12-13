#ifndef UKC_CLI_CPP_3AJJPELC
#define UKC_CLI_CPP_3AJJPELC

#include "PlatformInc.hpp"
#include "StringType.hpp"

#include <iostream>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <vector>
#include <iostream>
using std::vector;

#ifdef _WIN32
    static auto &Cout=std::wcout;
    static auto &Cin=std::wcin;
    #define MAIN_ENTRY wmain
#else
    static auto &Cout=std::cout;
    static auto &Cin=std::cin;
    #define MAIN_ENTRY main
#endif

int MAIN_ENTRY(int argc, StringType::value_type* argv[])
{

    Cout<<"scanning devices...\n";

    auto list=InputDevice::scanDevices();

    if (list.empty()){
        
        Cout<<"can not found valid input device, maybe you need root permission\n";
        return EXIT_FAILURE;
    }
    
    
    for(size_t i=0;i<list.size();++i){
        Cout<<"index "<<i<<":"<<list[i]->description()<<"\n";
    }

    Cout<<"choose which devices you want to use as input source \n"
        <<"input format : index [index], for example: 0 1 4"
        <<"(their input will be grabbed exclusive\n"
        <<" used as virtual unified keyboard input)\n"
        <<"please note: dumb map can makes input device totally none-work";

    StringType selected;

    getline(Cin,selected);

    vector<StringType> selectedIndex;

    boost::split(selectedIndex,selected,boost::is_space(),
            boost::token_compress_on);

    return EXIT_SUCCESS;
}


#endif /* end of include guard: UKC_CLI_CPP_3AJJPELC */
