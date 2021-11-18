#include "knxutils.h"
#include "knxexception.h"
#include "wblib/utils.h"

uint8_t knx::utils::StringValueToByte(const std::string& byte)
{
    if (byte.empty())
        wb_throw(knx::TKnxException, "Trying to read a byte from empty string");

    try {
        if (byte.substr(0, 2) == "0b" || byte.substr(0, 2) == "0B") {
            if (byte.length() == 2)
                wb_throw(knx::TKnxException, "invalid byte: " + byte);
            char* strEnd;
            unsigned res = std::strtoul(byte.c_str() + 2, &strEnd, 2);
            if (*strEnd != 0)
                wb_throw(knx::TKnxException, "invalid byte: " + byte);
            return res;
        } else {
            std::size_t num;
            int ret = std::stoi(byte, &num, 0);
            if (num != byte.length())
                wb_throw(knx::TKnxException, "invalid byte: " + byte);
            return ret;
        }
    } catch (knx::TKnxException& e) {
        throw;
    } catch (std::exception& e) {
        wb_throw(knx::TKnxException, "invalid byte: " + byte);
    }
}
