#include "NotMatcher.hpp"

#include <boost/property_tree/ptree.hpp>

bool NotMatcher::matchDevice(const InputDevice &device)const{

    if(!m_value){
        //default not match anything
        return false;
    }

    return !m_value->matchDevice(device);

}

static const StringType VALUE_KEY=_T("value");

void NotMatcher::writeSelf(TreeType& writeTo) const {

    if(!m_value){
        return;
    }

    TreeType childTree;

    write(*m_value,childTree);

    writeTo.put_child(VALUE_KEY,childTree);
}

void NotMatcher::readSelf(const TreeType& readFrom) {


    //TODO
}


    
NotMatcher* NotMatcher::deepClone() const {

    NotMatcher *clone=new NotMatcher;

    if(m_value){
        clone->m_value.reset(m_value->deepClone());
    }

    return clone;
}

