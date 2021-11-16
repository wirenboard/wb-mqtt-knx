#pragma once

#include "../src/iknxgroupobjectcontroller.h"
#include "../src/knxgroupaddress.h"
#include "gmock/gmock.h"

class TKnxGroupObjectControllerMock: public knx::IKnxGroupObjectController
{
public:
    MOCK_METHOD2(AddGroupObject,
                 bool(const knx::TKnxGroupAddress& groupAddress,
                      const std::shared_ptr<knx::object::TGroupObjectBase>&));
    MOCK_METHOD1(RemoveGroupObject, bool(const knx::TKnxGroupAddress& a));

    virtual ~TKnxGroupObjectControllerMock() = default;
};