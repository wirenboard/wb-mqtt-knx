#include "knxlegacydevice.h"
#include "knxconverter.h"
#include "knxexception.h"

using namespace knx;

namespace
{
    constexpr auto DEVICE_ID = "knx";
    constexpr auto DEVICE_TITLE = "KNX gateway";
    constexpr auto DEVICE_TITLE_RU = "Шлюз KNX";
    constexpr auto CONTROL_ID = "data";
    constexpr auto CONTROL_TITLE = "Message";
    constexpr auto CONTROL_TITLE_RU = "Сообщение";
    constexpr auto CONTROL_TYPE = "text";
} // namespace

TKnxLegacyDevice::TKnxLegacyDevice(WBMQTT::PDeviceDriver pMqttDriver,
                                   knx::PSender<TTelegram> pKnxTelegramSender,
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

    LocalDevice = tx->CreateDevice(WBMQTT::TLocalDeviceArgs{}
                                       .SetId(DEVICE_ID)
                                       .SetTitle(DEVICE_TITLE, "en")
                                       .SetTitle(DEVICE_TITLE_RU, "ru")
                                       .SetIsVirtual(true)
                                       .SetDoLoadPrevious(false))
                      .GetValue();

    Control = LocalDevice
                  ->CreateControl(tx,
                                  WBMQTT::TControlArgs{}
                                      .SetId(CONTROL_ID)
                                      .SetTitle(CONTROL_TITLE, "en")
                                      .SetTitle(CONTROL_TITLE_RU, "ru")
                                      .SetType(CONTROL_TYPE)
                                      .SetError("")
                                      .SetOrder(0)
                                      .SetReadonly(false))
                  .GetValue();

    Control->SetOnValueReceiveHandler(
        [this](const WBMQTT::PControl& pControl, const WBMQTT::TAny& value, const WBMQTT::PDriverTx& tx) {
            const auto& valueStr = value.As<std::string>();
            DebugLogger.Log() << "On event message:\"" << valueStr << "\""
                              << " from control: " << pControl->GetId();
            try {
                auto telegram = knx::converter::MqttToKnxTelegram(valueStr);
                if (KnxTelegramSender)
                    KnxTelegramSender->Send(*telegram);
                else {
                    ErrorLogger.Log() << "TKnxLegacyDevice: KnxTelegramSender is nullptr";
                }
            } catch (const std::exception& e) {
                ErrorLogger.Log() << e.what();
            }
        });
}

void TKnxLegacyDevice::Deinit()
{
    try {
        DeviceDriver->BeginTx()->RemoveDeviceById(DEVICE_ID).Sync();
    } catch (const std::exception& e) {
        ErrorLogger.Log() << "Exception during TKnxLegacyDevice::Deinit: " << e.what();
    } catch (...) {
        ErrorLogger.Log() << "Unknown exception during TKnxLegacyDevice::Deinit";
    }
}

void TKnxLegacyDevice::Notify(const TKnxEvent&, const TTelegram& t)
{
    try {
        const auto mqttData = converter::KnxTelegramToMqtt(t);
        Control->UpdateRawValue(DeviceDriver->BeginTx(), mqttData).Wait();
    } catch (const std::exception& e) {
        ErrorLogger.Log() << e.what();
    }
}
