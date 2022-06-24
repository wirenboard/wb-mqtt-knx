#include "dpt20.h"
#include <limits>

using namespace knx::object;
std::vector<DptDescriptorField> TDpt20::getDescriptor() const
{
    return {{"Field1", "value", std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max()}};
}

TDatapointId TDpt20::GetId() const
{
    return TDatapointId{20};
}