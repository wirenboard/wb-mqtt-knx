#include "testutils.h"

namespace testUtils
{
    Json::Value ParseJson(const std::string& jsonStr)
    {
        std::stringstream sstr(jsonStr);
        Json::Value jsonObject;
        sstr >> jsonObject;
        return jsonObject;
    }
}
