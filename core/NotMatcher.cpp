#include "NotMatcher.hpp"

#include <boost/property_tree/ptree.hpp>

bool NotMatcher::matchDevice(const InputDevice &device)const{

    if(!m_value){
        //default not match anything
        return false;
    }

    return !m_value->matchDevice(device);

}

static const string VALUE_KEY="value";

void NotMatcher::writeSelf(ptree& writeTo) const {

    if(!m_value){
        return;
    }

    ptree childTree;

    write(*m_value,childTree);

    writeTo.put_child(VALUE_KEY,childTree);
}

void NotMatcher::readSelf(const ptree& readFrom) {


    //TODO
}


    
NotMatcher* NotMatcher::deepClone() const {

    NotMatcher *clone=new NotMatcher;

    if(m_value){
        clone->m_value.reset(m_value->deepClone());
    }

    return clone;
}

