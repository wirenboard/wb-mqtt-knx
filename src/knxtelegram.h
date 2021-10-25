#ifndef WB_MQTT_KNX_KNXTELEGRAM_H
#define WB_MQTT_KNX_KNXTELEGRAM_H

#include <eibclient.h>
#include <string>
#include <vector>

namespace knx
{
    namespace telegram
    {
        enum class TApci
        {
            /// Runtime APCI command
            GroupValueRead = 0x0,
            GroupValueResponse = 0x1,
            GroupValueWrite = 0x2,

            /// APCI command for configuring system
            IndividualAddrWrite = 0x3,
            IndividualAddrRequest = 0x4,
            IndividualAddrResponse = 0x5,
            AdcRead = 0x6,
            AdcResponse = 0x7,
            MemoryRead = 0x8,
            MemoryResponse = 0x9,
            MemoryWrite = 0xA,
            UserMessage = 0xB,
            MaskVersionRead = 0xC,
            MaskVersionResponse = 0xD,
            Restart = 0xE,
            Escape = 0xF
        };

        enum class TTypeOfTransportLayer
        {
            UDP = 0x0,
            NDP = 0x1,
            UCD = 0x2,
            NCD = 0x3
        };

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

        uint8_t GetAPDULength() const;

        telegram::TTypeOfTransportLayer GetTypeOfTransportLayer() const;
        void SetTypeOfTransportLayer(telegram::TTypeOfTransportLayer type);

        uint8_t GetSequenceNumber() const;
        void SetSequenceNumber(uint8_t number);

        telegram::TApci GetAPCI() const;
        void SetAPCI(telegram::TApci apci);

        std::vector<uint8_t> GetTPDUPayload() const;

        std::vector<uint8_t> GetAPDUPayload() const;
        void SetAPDUPayload(const std::vector<uint8_t>& payload);

        std::vector<uint8_t> GetRawTelegram() const;
        bool SetRawTelegram(const std::vector<uint8_t>& knxBuffer);

        static constexpr uint32_t MaxPayloadSize = 15;
        static constexpr uint32_t SizeWithoutPayload = 8;

    private:
        static uint8_t CalculateParity(const std::vector<uint8_t>& data);

        bool IsRepeatedTelegram{false};
        telegram::TPriority Priority{telegram::TPriority::Priority4_NormalMode_LowPriority};
        eibaddr_t SourceAddress{0};
        eibaddr_t ReceiverAddress{0};
        bool IsReceiverAddressGrouped{false};
        uint8_t RoutingCounter{0};
        telegram::TTypeOfTransportLayer TypeOfTransportLayer{telegram::TTypeOfTransportLayer::UDP};
        uint8_t SequenceNumber{0};
        telegram::TApci Apci{telegram::TApci::GroupValueRead};
        std::vector<uint8_t> ApduPayload{0};
    };

} // namespace knx

#endif // WB_MQTT_KNX_KNXTELEGRAM_H
