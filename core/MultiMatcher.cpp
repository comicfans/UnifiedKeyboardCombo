#include "MultiMatcher.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>

bool MultiMatcher::matchDevice(const InputDevice &device)const{

    if(m_values.empty()){
        return true;
    }

    for(auto &one:m_values){

        bool thisResult=one.matchDevice(device);

        if(m_operateMode==OR && thisResult){
            return true;
        }

        if(m_operateMode==AND && !thisResult){
            return false;
        }
        
    }

    //if over loop as AND check ,this is all match
    //if over loop as OR check ,this means no one match
    return m_operateMode==AND;

}

static const string OPERATE_MODE_KEY="operateMode";

static const string VALUES_KEY="values";

static const string MATCHER_KEY="matcher";

static const string OPERATE_MODE_MAP_NAME[2]={"AND","OR"};

void MultiMatcher::writeSelf(ptree& writeTo) const {

    writeTo.put(OPERATE_MODE_KEY,OPERATE_MODE_MAP_NAME[m_operateMode]);

    int index=0;

    ptree values;

    for(auto &child : m_values){
        ptree childTree;

        write(child,childTree);

        values.add_child(MATCHER_KEY,childTree);
    }

    writeTo.put_child(VALUES_KEY,values);

}

void MultiMatcher::readSelf(const ptree& readFrom) {

    string operateMode=readFrom.get<string>(OPERATE_MODE_KEY);

    if(operateMode=="AND"){
        m_operateMode=AND;
    }else{
        m_operateMode=OR;
    }

    auto &valuesTree=readFrom.get_child(VALUES_KEY);

    for(auto &child:valuesTree){

        auto value=DeviceMatcher::read(child.second);

        m_values.insert(value);
    }
}


void MultiMatcher::addMatcher(DeviceMatcher * token){

    m_values.insert(token);
}

        
std::size_t MultiMatcher::Hasher::operator()(const DeviceMatcher& value)const{

    boost::hash<const void*> hasher;
    return hasher(&value);
}
        
bool MultiMatcher::Equaler::operator()(
        const DeviceMatcher &lhs,const DeviceMatcher &rhs)const{
    return &lhs==&rhs;
}
    
MultiMatcher* MultiMatcher::deepClone()const {

    MultiMatcher *clone=new MultiMatcher;

    clone->m_operateMode=m_operateMode;

    for(auto &value:m_values){
        clone->m_values.insert(value.deepClone());
    }

    return clone;

}

