/*
 * =====================================================================================
 *
 *       Filename:  MultiMatcher.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/28/2014 04:40:10 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef MULTIMATCHER_HPP_XOL4QWUQ
#define MULTIMATCHER_HPP_XOL4QWUQ




#include "DeviceMatcher.hpp"

#include <boost/ptr_container/ptr_unordered_set.hpp>


class MultiMatcher :public DeviceMatcher
{

public:

    static constexpr const StringType::value_type* const CLASS_NAME=_T("MultiMatcher");

    virtual const StringType::value_type* const  className()const {return CLASS_NAME;}

    enum OperateMode{AND,OR};

    void setOperateMode(OperateMode value){m_operateMode=value;}

    OperateMode operateMode()const{return m_operateMode;}

    void addMatcher(DeviceMatcher *taken);

    virtual bool matchDevice(const InputDevice& inputDevice)const override final;

    virtual MultiMatcher* deepClone()const override final;

protected:

    virtual void writeSelf(TreeType& writeTo) const override final;

    virtual void readSelf(const TreeType& readFrom) override final;


private:


    struct Hasher:public std::unary_function<DeviceMatcher,std::size_t>{
        std::size_t operator()(const DeviceMatcher& value)const;
    };

    struct Equaler:public std::binary_function<DeviceMatcher,DeviceMatcher,bool>{
        bool operator()(const DeviceMatcher &lhs,const DeviceMatcher &rhs)const;
    };

    OperateMode m_operateMode = AND;

    boost::ptr_unordered_set<DeviceMatcher,Hasher,Equaler> m_values;

};

#endif /* end of include guard: MULTIMATCHER_HPP_XOL4QWUQ */
