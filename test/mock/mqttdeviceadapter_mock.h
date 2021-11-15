#pragma once

#include "../src/mqtt/imqttdeviceadapter.h"
#include "gmock/gmock.h"

class TMqttDeviceAdapterBuilderMock: public mqtt::IMqttDeviceAdapterBuilder
{
public:
    MOCK_METHOD1(SetDeviceId, IMqttDeviceAdapterBuilder&(const std::string& name));
    MOCK_METHOD1(SetDeviceName, IMqttDeviceAdapterBuilder&(const std::string& name));
    MOCK_METHOD0(Create, std::shared_ptr<mqtt::IMqttDeviceAdapter>());

    virtual ~TMqttDeviceAdapterBuilderMock() = default;
};

class TMqttDeviceAdapterMock: public mqtt::IMqttDeviceAdapter
{
public:
    MOCK_METHOD5(CreateControl,
                 std::shared_ptr<mqtt::IMqttControlAdapter>(const std::string& id,
                                                            const std::string& type,
                                                            const std::string& name,
                                                            double min,
                                                            double max));
    virtual ~TMqttDeviceAdapterMock() = default;
};