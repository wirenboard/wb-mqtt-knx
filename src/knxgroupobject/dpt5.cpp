#include "dpt5.h"

using namespace knx::object;
std::vector<DptDescriptorField> TDpt5::getDescriptor() const
{
    return {{"UnsignedValue", "value", 0, 255}};
}
