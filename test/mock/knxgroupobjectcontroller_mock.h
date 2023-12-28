#pragma once

#include "../src/iknxgroupobjectcontroller.h"
#include "../src/knxgroupaddress.h"
#include <gmock/gmock.h>

class TKnxGroupObjectControllerMock: public knx::IKnxGroupObjectController
{
public:
    MOCK_METHOD2(AddGroupObject, bool(const knx::object::PGroupObject&, const knx::TGroupObjectSettings&));
    MOCK_METHOD1(RemoveGroupObject, bool(const knx::TKnxGroupAddress& a));

    virtual ~TKnxGroupObjectControllerMock() = default;
};