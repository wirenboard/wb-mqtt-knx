#include <eibclient.h>
#include <memory>
#include <string>

#ifndef KNX_CLIENT_H
#define KNX_CLIENT_H

class TMqttKnxObserver;
typedef std::shared_ptr<TMqttKnxObserver> PMqttKnxObserver;

#define MAX_TELEGRAM_LENGTH 254

class TKnxException : public std::exception {
public:
    TKnxException(std::string message) : Message("KNX error: " + message)
    {
    }
    const char* what() const throw()
    {
        return Message.c_str();
    }

private:
    std::string Message;
};

class TKnxClient {
public:
    TKnxClient(std::string url);
    void SendTelegram(std::string payload);
    void Loop();
    virtual ~TKnxClient();
    void Observe(PMqttKnxObserver observer);
    bool SetDebug(bool debug);
    static eibaddr_t ParseKnxAddress(const std::string& addr);

private:
    EIBConnection* Out;
    EIBConnection* In;
    PMqttKnxObserver Observer;
    bool Debug;
};
typedef std::shared_ptr<TKnxClient> PKnxClient;

#endif // KNX_CLIENT_H
