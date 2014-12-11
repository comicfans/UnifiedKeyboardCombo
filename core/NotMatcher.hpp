#ifndef NOTMATCHER_HPP_W3CYLAUP
#define NOTMATCHER_HPP_W3CYLAUP

#include "DeviceMatcher.hpp"

#include <memory>

class NotMatcher: public DeviceMatcher
{
public:

    static constexpr const StringType::value_type* const CLASS_NAME=_T("NotMatcher");

    virtual const StringType::value_type* const className()const override final{
        return CLASS_NAME;
    }

    virtual NotMatcher* deepClone() const override final;

    void setMatcher(DeviceMatcher * token){
        m_value.reset(token);
    }

    virtual bool matchDevice(const InputDevice& inputDevice)const override final;

protected:

    virtual void writeSelf(TreeType& writeTo) const override final;

    virtual void readSelf(const TreeType& readFrom) override final;

private:

    std::unique_ptr<DeviceMatcher> m_value;

};


#endif /* end of include guard: NOTMATCHER_HPP_W3CYLAUP */
