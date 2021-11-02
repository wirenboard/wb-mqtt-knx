#pragma once

#include "base.h"
#include "ifactory.h"

namespace knx
{
    namespace object
    {
        class TGroupObjectMqtt: public TGroupObjectBase
        {
        public:
            TGroupObjectMqtt(const TKnxGroupAddress& address, const ISenderGroupObject& sender)
                : TGroupObjectBase(address, sender)
            {}
            void Notify(const TGroupObjectTransaction&) override;

            virtual ~TGroupObjectMqtt() = default;
        };

        class TGroupObjectMqttFactory: public IGroupObjectFactory
        {
        public:
            std::shared_ptr<TGroupObjectBase> Create(const TKnxGroupAddress& address,
                                                     const ISenderGroupObject& groupObject) const override;
        };
    }
}
