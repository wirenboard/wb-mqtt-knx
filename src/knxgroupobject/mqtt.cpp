#include "mqtt.h"

using namespace knx::object;

void TGroupObjectMqtt::Notify(const TGroupObjectTransaction& transaction)
{
    // TODO!!!
}

std::shared_ptr<TGroupObjectBase> TGroupObjectMqttFactory::Create(const knx::TKnxGroupAddress& address,
                                                                  const ISenderGroupObject& groupObject) const
{
    return std::make_shared<TGroupObjectMqtt>(address, groupObject);
}
