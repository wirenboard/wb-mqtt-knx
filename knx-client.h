#include <eibclient.h>
#include <memory>
#include <string>

#ifndef KNX_CLIENT_H
#define KNX_CLIENT_H

class TMqttKnxObserver;
typedef std::shared_ptr<TMqttKnxObserver> PMqttKnxObserver;

#define MAX_TELEGRAM_LENGTH 254

class TKnxClient {
public:
    TKnxClient();
    void SendTelegram(std::string payload);
    void Loop();
    virtual ~TKnxClient();
    void Observe(PMqttKnxObserver observer);
    bool SetDebug(bool debug);

private:
    EIBConnection* Out;
    EIBConnection* In;
    PMqttKnxObserver Observer;
    bool Debug;
};
typedef std::shared_ptr<TKnxClient> PKnxClient;

#endif // KNX_CLIENT_H
