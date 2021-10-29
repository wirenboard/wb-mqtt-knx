#ifndef WB_MQTT_KNX_KNXTELEGRAMTPDU_H
#define WB_MQTT_KNX_KNXTELEGRAMTPDU_H

#include <string>
#include <vector>

namespace knx
{
    namespace telegram
    {
        enum class TCommunicationType
        {
            UDP = 0x0,
            NDP = 0x1,
            UCD = 0x2,
            NCD = 0x3
        };

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

        enum class TControlDataType
        {
            Connect = 0x0,
            Disconnect = 0x1,
            Ack = 0x2,
            Nack = 0x3
        };
    }

    class TTpdu
    {
    public:
        explicit TTpdu() = default;
        explicit TTpdu(const std::vector<uint8_t>& tpduBuffer);

        uint8_t GetPayloadLength() const;

        telegram::TCommunicationType GetCommunicationType() const;
        void SetCommunicationType(telegram::TCommunicationType type);

        uint8_t GetSequenceNumber() const;
        void SetSequenceNumber(uint8_t number);

        telegram::TApci GetAPCI() const;
        void SetAPCI(telegram::TApci apci);

        telegram::TControlDataType GetControlDataType() const;
        void SetControlDataType(telegram::TControlDataType type);

        std::vector<uint8_t> GetPayload() const;
        void SetPayload(const std::vector<uint8_t>& payload);
        void SetPayload(std::vector<uint8_t>::const_iterator beginIt, std::vector<uint8_t>::const_iterator endIt);

        std::vector<uint8_t> GetRaw() const;
        void SetRaw(const std::vector<uint8_t>& tpduBuffer);
        void SetRaw(std::vector<uint8_t>::const_iterator beginIt, std::vector<uint8_t>::const_iterator endIt);

        uint32_t Size() const;

        static constexpr uint32_t MaxPayloadSize = 15;

    private:
        telegram::TCommunicationType CommunicationType{telegram::TCommunicationType::UDP};
        uint8_t SequenceNumber{0};
        telegram::TApci Apci{telegram::TApci::GroupValueRead};
        std::vector<uint8_t> ApduPayload{0};
        telegram::TControlDataType CDataType{telegram::TControlDataType::Connect};
    };
}

#endif // WB_MQTT_KNX_KNXTELEGRAMTPDU_H
