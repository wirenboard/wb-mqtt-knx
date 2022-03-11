#include "datapointid.h"
#include <regex>
namespace knx
{
    namespace object
    {
        TDatapointId::TDatapointId(uint32_t main): Main(main), HasSub(false)
        {}

        TDatapointId::TDatapointId(uint32_t main, uint32_t sub): Main(main), Sub(sub), HasSub(true)
        {}

        uint32_t TDatapointId::GetMain() const
        {
            return Main;
        }

        void TDatapointId::SetMain(uint32_t main)
        {
            Main = main;
        }

        uint32_t TDatapointId::GetSub() const
        {
            return Sub;
        }

        void TDatapointId::SetSub(uint32_t sub)
        {
            Sub = sub;
            HasSub = true;
        }

        uint32_t TDatapointId::HasSubId() const
        {
            return HasSub;
        }

        bool TDatapointId::SetFromString(const std::string& stringIndex)
        {
            std::regex indexRegex("([0-9]+)\\.([xX0-9]+)");
            std::regex hasNotSubIndex("[xX]+");
            std::smatch sm;

            if (std::regex_search(stringIndex, sm, indexRegex)) {
                Main = std::stoul(sm.str(1));
                if (!std::regex_search(sm.str(2), hasNotSubIndex)) {
                    SetSub(std::stoul(sm.str(2)));
                }
                return true;
            }
            return false;
        }

        std::string TDatapointId::ToString() const
        {
            std::stringstream ss;
            ss << Main;
            if (HasSub) {
                ss << "." << Sub;
            } else {
                ss << ".xxx";
            }
            return ss.str();
        }
    }
}
