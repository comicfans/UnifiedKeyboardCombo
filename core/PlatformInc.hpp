#ifndef PLATFORMINC_HPP_LZBYTHV7
#define PLATFORMINC_HPP_LZBYTHV7

#ifdef _WIN32
    #include "win32/RawInputDevice.hpp"
#elif __linux
    #include "linux/EvdevInputDevice.hpp"
#endif

#include "PlatformDef.hpp"




#endif /* end of include guard: PLATFORMINC_HPP_LZBYTHV7 */
