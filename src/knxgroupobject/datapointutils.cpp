#include "datapointutils.h"
#include <limits>
#include <vector>

namespace
{
    static_assert(std::numeric_limits<float>::is_iec559, "Float is not IEC 559/IEEE 754 compliant");

    union
    {
        uint32_t integerValue;
        float floatValue;
    } floatConverter;

}

namespace knx
{
    namespace object
    {
        double datapointUtils::RawToFloat16(uint16_t raw)
        {
            bool isNegative = raw & (1 << 15);
            auto mantissa = static_cast<int16_t>((isNegative ? (0x1F << 11) : 0) | (raw & 0x7FF));
            uint32_t exp = (raw >> 11) & 0x0F;
            return (static_cast<double>(mantissa) * 0.01) * std::pow(2, exp);
        }

        uint16_t datapointUtils::FloatToRaw16(double value)
        {
            auto mantissa = static_cast<int32_t>(std::round(value * 100));
            uint32_t exp = 0;
            while ((mantissa < -2048) || (mantissa > 2047)) {
                mantissa >>= 1;
                ++exp;
            }

            return (static_cast<int16_t>(mantissa) & 0x87FF) | ((exp & 0x0F) << 11);
        }

        float datapointUtils::RawToFloat32(uint32_t raw)
        {
            floatConverter.integerValue = raw;
            return floatConverter.floatValue;
        }

        uint32_t datapointUtils::FloatToRaw32(float value)
        {
            floatConverter.floatValue = value;
            return floatConverter.integerValue;
        }
    }
}
