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
    //#include <fcntl.h>
    //#include <io.h>
    //static auto &Cout=std::wcout;
    //static auto &Cin=std::wcin;
    #define MAIN_ENTRY wmain
#else
    #define MAIN_ENTRY main
#endif

    static auto &Cout=std::cout;
    static auto &Cin=std::cin;

int MAIN_ENTRY(int argc, StringType::value_type* argv[])
{

    Cout<<"scanning devices...\n";

    auto list=InputDevice::scanDevices();

    if (list.empty()){
        
        Cout<<"can not found valid input device, maybe you need root permission\n";
        return EXIT_FAILURE;
    }
    
    
    for(size_t i=0;i<list.size();++i){
        auto desc=list[i]->description();
        auto length=desc.length()*4;
        char converted[length];
        std::wcstombs(converted,desc.data(),length);   
        Cout<<"index "<<i<<":"<<converted<<std::endl;
    }

    Cout<<"choose which devices you want to use as input source \n"
        <<"input format : index [index], for example: 0 1 4"
        <<"(their input will be grabbed exclusive\n"
        <<" used as virtual unified keyboard input)\n"
        <<"please note: dumb map can makes input device totally none-work";

    std::string selected;

    getline(Cin,selected);

    vector<StringType> selectedIndex;

    boost::split(selectedIndex,selected,boost::is_space(),
            boost::token_compress_on);

    return EXIT_SUCCESS;
}


#endif /* end of include guard: UKC_CLI_CPP_3AJJPELC */
