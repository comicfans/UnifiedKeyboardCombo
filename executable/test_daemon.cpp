#include "PlatformInc.hpp"
#include "DeviceMatcher.hpp"
#include "MainLoop.hpp"


int main(int argc, char *argv[])
{

    MainLoop::instance().start();
    
    return 0;
}
