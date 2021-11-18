#pragma once

#include "../../src/knxgroupobject/igroupobject.h"
#include "../../src/knxgroupobject/imqttbuilder.h"
#include "gmock/gmock.h"

class TGroupObjectMqttBuilderMock: public knx::object::IGroupObjectMqttBuilder
{
public:
    MOCK_METHOD2(LinkDevice, void(const std::string& id, const std::string& name));
    MOCK_METHOD1(Create,
                 std::shared_ptr<knx::object::IGroupObject>(const knx::object::TGroupObjectMqttParameter& parameter));

    MOCK_METHOD0(RemoveUnusedControls, void());
    MOCK_METHOD0(Clear, void());

    virtual ~TGroupObjectMqttBuilderMock() = default;
};