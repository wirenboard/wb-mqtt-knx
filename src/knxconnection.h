#pragma once

#include <eibclient.h>
#include <string>

namespace knx
{
    // wrapper for EIBConnection
    class TKnxConnection
    {
    public:
        explicit TKnxConnection(const std::string& knxServerUrl);

        EIBConnection* GetEIBConnection() const;

        bool operator!() const;

        ~TKnxConnection();

    private:
        EIBConnection* Connection;
    };
}
