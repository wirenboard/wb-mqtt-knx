#include "dpt5.h"
#include <limits>

using namespace knx::object;
std::vector<DptDescriptorField> TDpt5::getDescriptor() const
{
    return {{"UnsignedValue", "value", std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max()}};
}
