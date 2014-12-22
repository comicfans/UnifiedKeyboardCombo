#ifndef KEYMAP_HPP_96OYYFZW
#define KEYMAP_HPP_96OYYFZW

#include <string>

struct KeyMap{

    std::string fromKey;

    unsigned int fromKeyCode()const; 

    std::string toKey;

    unsigned int toKeyCode()const; 

    bool isMouseRel()const; 
};


#endif /* end of include guard: KEYMAP_HPP_96OYYFZW */
