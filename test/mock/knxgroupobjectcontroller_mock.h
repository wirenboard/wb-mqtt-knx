#pragma once

#include "../src/iknxgroupobjectcontroller.h"
#include "../src/knxgroupaddress.h"
#include "gmock/gmock.h"

class TKnxGroupObjectControllerMock: public knx::IKnxGroupObjectController
{
public:
    MOCK_METHOD3(AddGroupObject,
                 bool(const knx::TKnxGroupAddress& groupAddress,
                      const knx::object::PGroupObject&,
                      std::chrono::milliseconds));
    MOCK_METHOD1(RemoveGroupObject, bool(const knx::TKnxGroupAddress& a));

    virtual ~TKnxGroupObjectControllerMock() = default;
};