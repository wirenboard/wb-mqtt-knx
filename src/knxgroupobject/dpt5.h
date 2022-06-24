#pragma once
#include "dptbaseu8.h"

namespace knx
{
    namespace object
    {
        /// \brief Datapoint Types "8-Bit Unsigned Value"
        ///
        /// - ID: 5.xxx
        /// - Format: 8 bit: U8
        /// - Field names: UnsignedValue
        class TDpt5: public TDptBaseU8
        {
        public:
            TDatapointId GetId() const override;
            std::vector<DptDescriptorField> getDescriptor() const override;
        };
    }
}
