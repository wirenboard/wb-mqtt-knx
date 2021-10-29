#ifndef WB_MQTT_KNX_KNXTELEGRAM_H
#define WB_MQTT_KNX_KNXTELEGRAM_H

#include "knxtelegramtpdu.h"
#include <eibclient.h>
#include <string>
#include <vector>

namespace knx
{
    namespace telegram
    {
        enum class TPriority
        {
            Priority1_SystemFunctions = 0x0,
            Priority2_AlarmFunctions = 0x1,
            Priority3_NormalMode_HighPriority = 0x2,
            Priority4_NormalMode_LowPriority = 0x3,
        };
    } // namespace telegram

    class TTelegram
    {
    public:
        explicit TTelegram() = default;
        explicit TTelegram(const std::vector<uint8_t>& knxBuffer);

        void SetRepeated(bool isRepeated);
        bool IsRepeated() const;

        void SetPriority(telegram::TPriority priority);
        telegram::TPriority GetPriority() const;

        eibaddr_t GetSourceAddress() const;
        void SetSourceAddress(eibaddr_t addr);

        eibaddr_t GetReceiverAddress() const;
        void SetReceiverAddress(eibaddr_t addr);

        bool IsGroupAddressed() const;
        void SetGroupAddressedFlag(bool groupAddressed);

        uint8_t GetRoutingCounter() const;
        void SetRoutingCounter(uint8_t counter);

        std::vector<uint8_t> GetRawTelegram() const;
        void SetRawTelegram(const std::vector<uint8_t>& knxBuffer);

        TTpdu& Tpdu();
        const TTpdu& Tpdu() const;

        static constexpr uint32_t SizeWithoutPayload = 8;

    private:
        static uint8_t CalculateParity(const std::vector<uint8_t>& data);

        bool IsRepeatedTelegram{false};
        telegram::TPriority Priority{telegram::TPriority::Priority4_NormalMode_LowPriority};
        eibaddr_t SourceAddress{0};
        eibaddr_t ReceiverAddress{0};
        bool IsReceiverAddressGrouped{false};
        uint8_t RoutingCounter{0};

        TTpdu Mtpdu;
    };

} // namespace knx

#endif // WB_MQTT_KNX_KNXTELEGRAM_H
