#include <eibclient.h>
#include <memory>
#include <string>

#ifndef KNX_CLIENT_H
#define KNX_CLIENT_H

class TMqttKnxObserver;
typedef std::shared_ptr<TMqttKnxObserver> PMqttKnxObserver;

#define MAX_TELEGRAM_LENGTH 254
#define ERROR_TIMEOUT 30

class TKnxException : public std::exception {
public:
    TKnxException(std::string message) : Message("KNX error: " + message)
    {
    }
    const char* what() const throw()
    {
        return Message.c_str();
    }
    ~TKnxException() throw()
    {
    }

private:
    std::string Message;
};

class TKnxClient {

    // wrapper for EIBConnection
    class TKnxConnection {
    public:
        TKnxConnection(std::string url)
        {
            Connection = EIBSocketURL(url.c_str());
        }

        operator EIBConnection*()
        {
            return Connection;
        }

        bool operator!()
        {
            return !Connection;
        }

        ~TKnxConnection()
        {
            EIBClose(Connection);
        }

    private:
        EIBConnection* Connection;
    };

public:
    TKnxClient(std::string url);
    void SendTelegram(std::string payload);
    void Loop();
    virtual ~TKnxClient();
    void Observe(PMqttKnxObserver observer);
    static eibaddr_t ParseKnxAddress(const std::string& addr, bool isGroup);
    void HandleLoopError(std::string what, unsigned int timeout);

private:
    std::string Url;
    PMqttKnxObserver Observer;
};
typedef std::shared_ptr<TKnxClient> PKnxClient;

#endif // KNX_CLIENT_H
