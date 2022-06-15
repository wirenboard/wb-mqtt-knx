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

            std::experimental::optional<PDpt> Create(const TDatapointId& datapointId) override;

            bool HasDpt(const TDatapointId& datapointId) const override;

            std::experimental::optional<std::string> GetDptConfigName(const TDatapointId& datapointId) const override;

        private:
            Json::Value DescriptorRoot;
            std::map<uint32_t, Json::Value> DescriptorMap;
            std::map<TDatapointId, std::string> DatapointNameMap;
            void DatapointNameMapUpdate();
            std::experimental::optional<std::pair<knx::object::TDatapointId, std::string>> FindItem(
                const TDatapointId& datapointId) const;
        };

    }
}