#pragma once

#pragma once
#include "knxgroupaddress.h"
#include "knxgroupobject/base.h"
#include "knxtelegram.h"
#include <map>
#include <memory>
#include <utility>

namespace knx
{
    class IKnxGroupObjectController
    {
    public:
        virtual bool AddGroupObject(const knx::TKnxGroupAddress& groupAddress,
                                    const std::shared_ptr<object::TGroupObjectBase>& groupObject) = 0;
        virtual bool RemoveGroupObject(const TKnxGroupAddress& address) = 0;
    };
}
