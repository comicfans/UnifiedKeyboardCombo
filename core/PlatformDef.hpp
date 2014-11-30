/*
 * =====================================================================================
 *
 *       Filename:  PlatformDef.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年11月29日 14时54分26秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef PLATFORMDEF_HPP_9DF8CBTE
#define PLATFORMDEF_HPP_9DF8CBTE

#ifdef _WIN32
    class RawInputDevice;
    typedef RawInputDevice InputDevice;
#elif __linux
    class EvdevInputDevice;
    typedef EvdevInputDevice InputDevice;
#endif


#endif /* end of include guard: PLATFORMDEF_HPP_9DF8CBTE */
