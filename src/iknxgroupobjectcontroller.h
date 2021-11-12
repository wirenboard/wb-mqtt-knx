#pragma once

#pragma once
#include "knxgroupaddress.h"
#include "knxgroupobject/base.h"
#include "knxgroupobject/ifactory.h"
#include "knxtelegram.h"
#include <map>
#include <memory>
#include <utility>

namespace knx
{
    class IKnxGroupObjectController
    {
    public:
        virtual bool AddGroupObject(const std::shared_ptr<object::TGroupObjectBase>& groupObject) = 0;
        virtual bool RemoveGroupObject(const TKnxGroupAddress& address) = 0;
    };
}
