#ifndef KEYMAP_HPP_96OYYFZW
#define KEYMAP_HPP_96OYYFZW

#include <string>

struct KeyMap{

    std::string fromKey;

    unsigned int fromKeyCode()const{return nameToCode(fromKey);}

    std::string toKey;

    unsigned int toKeyCode()const{return nameToCode(toKey);}

    static unsigned int nameToCode(const std::string& name);

    static std::string codeToName(unsigned int code);
};


#endif /* end of include guard: KEYMAP_HPP_96OYYFZW */
