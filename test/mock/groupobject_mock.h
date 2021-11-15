#pragma once

#include "../../src/knxgroupobject/base.h"
#include "gmock/gmock.h"

class TGroupObjectMock: public knx::object::TGroupObjectBase
{
public:
    MOCK_METHOD1(KnxNotify, void(const std::vector<uint8_t>& payload));
};