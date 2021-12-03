#pragma once

#include <eibclient.h>
#include <string>

namespace knx
{
    /// Wrapper for EIBConnection
    class TKnxConnection
    {
    public:
        /// Constructor
        /// \param knxServerUrl the knxd server URL
        explicit TKnxConnection(const std::string& knxServerUrl);

        /// EIB Connection getter
        /// \return pointer of the EIB connection
        EIBConnection* GetEIBConnection() const;

        /// Connection check
        /// \return The connection has been established
        bool IsConnected() const;

        /// Destructor
        ~TKnxConnection();

    private:
        EIBConnection* Connection;
    };
}
