#ifndef UKC_CLI_CPP_3AJJPELC
#define UKC_CLI_CPP_3AJJPELC

#include "PlatformInc.hpp"

#include <iostream>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <vector>

using std::cout;
using std::cin;
using std::vector;

int main(int argc, char *argv[])
{

    cout<<"scanning devices...\n";

    auto list=InputDevice::scanDevices();

    if (list.empty()){
        
        cout<<"can not found valid input device, maybe you need root permission\n";
        return EXIT_FAILURE;
    }
    
    
    for(int i=0;i<list.size();++i){
        cout<<"index "<<i<<":"<<list[i]->description()<<"\n";
    }

    cout<<"choose which devices you want to use as input source \n"
        <<"input format : index [index], for example: 0 1 4"
        <<"(their input will be grabbed exclusive\n"
        <<" used as virtual unified keyboard input)\n"
        <<"please note: dumb map can makes input device totally none-work";

    string selected;

    getline(cin,selected);

    vector<string> selectedIndex;

    boost::split(selectedIndex,selected,boost::is_space(),
            boost::token_compress_on);

}


#endif /* end of include guard: UKC_CLI_CPP_3AJJPELC */
