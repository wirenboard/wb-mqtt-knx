#pragma once

#include "imqttdeviceadapter.h"
#include <wblib/wbmqtt.h>

#include <utility>

namespace mqtt
{
    class MqttDeviceAdapter: public IMqttDeviceAdapter
    {
    public:
        explicit MqttDeviceAdapter(std::shared_ptr<WBMQTT::TLocalDevice> pMqttDeviceDriver);
        std::shared_ptr<IMqttControlAdapter> CreateControl(const std::string& id,
                                                           const std::string& type,
                                                           const std::string& name,
                                                           double min,
                                                           double max) override;

        virtual ~MqttDeviceAdapter() = default;

    private:
        std::shared_ptr<WBMQTT::TLocalDevice> MqttLocalDevice;
    };

    class MqttDeviceAdapterBuilder: public IMqttDeviceAdapterBuilder
    {
    public:
        explicit MqttDeviceAdapterBuilder(std::shared_ptr<WBMQTT::TDeviceDriver> pMqttDeviceDriver)
            : MqttDeviceDriver(std::move(pMqttDeviceDriver))
        {}

        virtual ~MqttDeviceAdapterBuilder() = default;

        IMqttDeviceAdapterBuilder& SetDeviceId(const std::string& id) override;
        IMqttDeviceAdapterBuilder& SetDeviceName(const std::string& name) override;
        std::shared_ptr<IMqttDeviceAdapter> Create() override;

    private:
        std::shared_ptr<WBMQTT::TDeviceDriver> MqttDeviceDriver;

        std::string DeviceId;
        std::string DeviceName;
    };

}
