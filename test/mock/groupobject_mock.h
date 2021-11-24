#pragma once

#include "../../src/knxgroupobject/igroupobject.h"
#include "gmock/gmock.h"

class TGroupObjectMock: public knx::object::IGroupObject
{
public:
    MOCK_METHOD1(KnxNotify, void(const knx::object::TGroupObjectTransaction& transaction));
    MOCK_METHOD2(SetKnxSender,
                 void(const knx::TKnxGroupAddress& groupAddress,
                      knx::object::PSenderGroupObject sender));
};