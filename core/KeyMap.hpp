#ifndef KEYMAP_HPP_96OYYFZW
#define KEYMAP_HPP_96OYYFZW

#include "StringType.hpp"

struct KeyMap{

    StringType fromKey;

    unsigned int fromKeyCode()const{return nameToCode(fromKey);}

    StringType toKey;

    unsigned int toKeyCode()const{return nameToCode(toKey);}

    static unsigned int nameToCode(const StringType& name);

    static StringType codeToName(unsigned int code);
};


#endif /* end of include guard: KEYMAP_HPP_96OYYFZW */
