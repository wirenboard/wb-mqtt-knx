#pragma once
#include <memory>

#include "imqttcontroladapter.h"

namespace mqtt
{
    class IMqttDeviceAdapter
    {
    public:
        virtual std::shared_ptr<IMqttControlAdapter> CreateControl(const std::string& id,
                                                                   const std::string& type,
                                                                   const std::string& name,
                                                                   double min,
                                                                   double max) = 0;
    };

    class IMqttDeviceAdapterBuilder
    {
    public:
        virtual IMqttDeviceAdapterBuilder& SetDeviceId(const std::string& name) = 0;
        virtual IMqttDeviceAdapterBuilder& SetDeviceName(const std::string& name) = 0;

        virtual std::shared_ptr<IMqttDeviceAdapter> Create() = 0;
    };
}
