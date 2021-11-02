#pragma once

#include "base.h"

namespace knx
{
    namespace object
    {
        class IGroupObjectFactory
        {
        public:
            virtual std::shared_ptr<TGroupObjectBase> Create(const TKnxGroupAddress& address,
                                                             const ISenderGroupObject&) const = 0;
        };
    }
}
