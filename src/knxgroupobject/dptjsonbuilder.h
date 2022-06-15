#pragma once

#include "../knxexception.h"
#include "datapointid.h"
#include "dptjson.h"
#include "idpt.h"
#include "idptbuilder.h"
#include <bitset>
#include <regex>
#include <wblib/json_utils.h>

namespace knx
{
    namespace object
    {
        /// \brief Class for creating a JSON Datapoint based on the descriptor JSON file
        class TDptJsonBuilder: public IDptBuilder
        {
        public:
            /// Constructor
            /// \param descriptorPath Path to the JSON Datapoint Descriptor file
            explicit TDptJsonBuilder(const std::string& descriptorPath);

            PDpt Create(const TDatapointId& datapointId) override;

            bool HasDpt(const TDatapointId& datapointId) override;

            std::string GetDptName(const TDatapointId& datapointId) const;

        private:
            Json::Value DescriptorRoot;
            std::map<uint32_t, Json::Value> DescriptorMap;
            std::map<TDatapointId, std::string> DatapointNameMap;
            void DatapointNameMapUpdate();
        };

    }
}