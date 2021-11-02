#include "knxlegacydevice.h"
#include "knxconverter.h"
#include "knxexception.h"

using namespace knx;

namespace
{
    constexpr auto DEVICE_ID = "knx";
    constexpr auto CONTROL_ID = "data";
    constexpr auto CONTROL_TYPE = "text";
} // namespace

TKnxLegacyDevice::TKnxLegacyDevice(std::shared_ptr<WBMQTT::TDeviceDriver> pMqttDriver,
                       std::shared_ptr<knx::ISender<TTelegram>> pKnxTelegramSender,
                       WBMQTT::TLogger& errorLogger,
                       WBMQTT::TLogger& debugLogger,
                       WBMQTT::TLogger& infoLogger)
    : DeviceDriver(std::move(pMqttDriver)),
      KnxTelegramSender(std::move(pKnxTelegramSender)),
      ErrorLogger(errorLogger),
      DebugLogger(debugLogger),
      InfoLogger(infoLogger)
{

    auto tx = DeviceDriver->BeginTx();

    LocalDevice =
        tx->CreateDevice(WBMQTT::TLocalDeviceArgs{}.SetId(DEVICE_ID).SetIsVirtual(true).SetDoLoadPrevious(false))
            .GetValue();

    Control = LocalDevice
                  ->CreateControl(tx,
                                  WBMQTT::TControlArgs{}
                                      .SetId(CONTROL_ID)
                                      .SetType(CONTROL_TYPE)
                                      .SetError("")
                                      .SetOrder(0)
                                      .SetReadonly(false))
                  .GetValue();

    EventHandlerHandle =
        DeviceDriver->On<WBMQTT::TControlOnValueEvent>([this](const WBMQTT::TControlOnValueEvent& event) {
            DebugLogger.Log() << "On event message:\"" << event.RawValue << "\""
                              << " from control: " << event.Control->GetId();
            try {
                auto telegram = knx::converter::MqttToKnxTelegram(event.RawValue);
                KnxTelegramSender->Send(*telegram);
            } catch (const TKnxException& e) {
                ErrorLogger.Log() << e.what();
            } catch (const std::exception& e) {
                ErrorLogger.Log() << e.what();
            }
        });
}

void TKnxLegacyDevice::Deinit()
{
    DeviceDriver->RemoveEventHandler(EventHandlerHandle);

    try {
        DeviceDriver->BeginTx()->RemoveDeviceById(DEVICE_ID).Sync();
    } catch (const std::exception& e) {
        ErrorLogger.Log() << "Exception during TKnxLegacyDevice::Deinit: " << e.what();
    } catch (...) {
        ErrorLogger.Log() << "Unknown exception during TKnxLegacyDevice::Deinit";
    }
}

void TKnxLegacyDevice::Notify(const TTelegram& telegram)
{
    try {
        const auto mqttData = converter::KnxTelegramToMqtt(telegram);
        Control->UpdateRawValue(DeviceDriver->BeginTx(), mqttData).Wait();
    } catch (const TKnxException& e) {
        ErrorLogger.Log() << e.what();
    } catch (const std::exception& e) {
        ErrorLogger.Log() << e.what();
    }
}