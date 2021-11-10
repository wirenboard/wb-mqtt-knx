#include "mqttdeviceadapter.h"
#include "mqttcontroladapter.h"
using namespace mqtt;

std::shared_ptr<IMqttControlAdapter> MqttDeviceAdapter::CreateControl(const std::string& id,
                                                                      const std::string& type,
                                                                      const std::string& name,
                                                                      double min,
                                                                      double max)
{
    auto localControl = MqttLocalDevice
                            ->CreateControl(MqttLocalDevice->GetDriver()->BeginTx(),
                                            WBMQTT::TControlArgs{}
                                                .SetId(id)
                                                .SetTitle(name)
                                                .SetType(type)
                                                .SetMin(min)
                                                .SetMax(max)
                                                .SetError("")
                                                .SetOrder(0)
                                                .SetReadonly(false))
                            .GetValue();
    return std::make_shared<MqttControlAdapter>(localControl);
}

MqttDeviceAdapter::MqttDeviceAdapter(std::shared_ptr<WBMQTT::TLocalDevice> pMqttDeviceDriver)
    : MqttLocalDevice(std::move(pMqttDeviceDriver))
{}

IMqttDeviceAdapterBuilder& MqttDeviceAdapterBuilder::SetDeviceId(const std::string& id)
{
    DeviceId = id;
    return *this;
}

IMqttDeviceAdapterBuilder& MqttDeviceAdapterBuilder::SetDeviceName(const std::string& name)
{
    DeviceName = name;
    return *this;
}

std::shared_ptr<IMqttDeviceAdapter> MqttDeviceAdapterBuilder::Create()
{
    auto localDevice = MqttDeviceDriver->BeginTx()
                           ->CreateDevice(WBMQTT::TLocalDeviceArgs{}
                                              .SetId(DeviceId)
                                              .SetTitle(DeviceName)
                                              .SetIsVirtual(true)
                                              .SetDoLoadPrevious(false))
                           .GetValue();
    return std::make_shared<MqttDeviceAdapter>(localDevice);
}
