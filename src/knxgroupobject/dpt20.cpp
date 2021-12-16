#include "dpt20.h"

using namespace knx::object;
std::vector<DptDescriptorField> TDpt20::getDescriptor() const
{
    return {{"Field1", "value", 0, 255}};
}
