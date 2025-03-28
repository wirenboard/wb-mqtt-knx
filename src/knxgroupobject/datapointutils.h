#pragma once

#include <bitset>
#include <complex>
#include <cstdint>

namespace knx
{
    namespace object
    {
        namespace datapointUtils
        {
            double RawToFloat16(uint16_t raw);
            uint16_t FloatToRaw16(double value);
            float RawToFloat32(uint32_t raw);
            uint32_t FloatToRaw32(float value);
        }
    }
}
