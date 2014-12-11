/*
 * =====================================================================================
 *
 *       Filename:  StringType.hpp
 *
 *    Description:  
 *
 *
 *        Version:  1.0
 *        Created:  2014年12月11日 22时13分37秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef STRINGTYPE_HPP_ZB9ONXIJ
#define STRINGTYPE_HPP_ZB9ONXIJ

#include <string>

#ifdef _WIN32
    #include <tchar.h>
    using StringType=std::wstring;
#else
    using StringType=std::string;
    #define _T 
#endif


#endif /* end of include guard: STRINGTYPE_HPP_ZB9ONXIJ */
