#include "knxconnection.h"

using namespace knx;

TKnxConnection::TKnxConnection(const std::string& knxServerUrl)
{
    Connection = EIBSocketURL(knxServerUrl.c_str());
}
EIBConnection* TKnxConnection::GetEIBConnection() const
{
    return Connection;
}
bool TKnxConnection::operator!() const
{
    return !Connection;
}
TKnxConnection::~TKnxConnection()
{
    EIBClose(Connection);
}
