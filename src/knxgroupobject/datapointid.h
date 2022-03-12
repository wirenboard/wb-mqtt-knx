#pragma once
#include <stdint.h>
#include <string>

namespace knx
{
    namespace object
    {
        /// Datapoint ID Class
        class TDatapointId
        {
        public:
            /// Default constructor
            explicit TDatapointId() = default;

            /// Constructor without sub ID (ex. 1.xxx)
            /// \param main main ID
            explicit TDatapointId(uint32_t main);

            /// Constructor with sub ID (ex. 1.001)
            /// \param main main ID
            /// \param sub sub ID
            TDatapointId(uint32_t main, uint32_t sub);

            /// Getter Main ID
            /// \return Main ID
            uint32_t GetMain() const;

            /// Setter Main ID
            /// \param main Main ID
            void SetMain(uint32_t main);

            /// Getter Sub ID
            /// \return Sub ID
            uint32_t GetSub() const;

            /// Setter Sub ID
            /// \param sub Sub ID
            void SetSub(uint32_t sub);

            /// Is there a Sub ID
            /// \return there is
            uint32_t HasSubId() const;

            /// Set datapoint Id from string
            /// \param stringId String like "1.xxx_text" "1.x" "1.001" "1.001_text" <br>
            ///                 If there is at least one lowercase and uppercase X character after the dot,<br>
            ///                 then the ID is counted without sub-index
            /// \return String processing was successful
            bool SetFromString(const std::string& stringId);

            /// Get datapoint ID string
            /// \return String in format "1.xxx" "1.1"
            std::string ToString() const;

        private:
            uint32_t Main{};
            uint32_t Sub{};
            bool HasSub{};
        };
    }
}
