#pragma once
#include "dptbaseu8.h"

namespace knx
{
    namespace object
    {
        /// \brief Datapoint Types N8
        ///
        /// - ID: 20.xxx
        /// - Format: 1 octet: N8
        /// - Field names: field1
        class TDpt20: public TDptBaseU8
        {
        public:
            std::vector<DptDescriptorField> getDescriptor() const override;
        };
    }
}
