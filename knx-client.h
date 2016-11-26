#include <string>
#include <memory>
#include <eibclient.h>

#ifndef KNX_CLIENT_H
#define KNX_CLIENT_H

class TMQTTKnxObserver;
typedef std::shared_ptr<TMQTTKnxObserver> PMQTTKnxObserver;

#define MAX_PAYLOAD_SIZE 254
class TKNXClient {
public:
    TKNXClient();
    void Send(std::string topic, std::string payload);
    void Loop();
    ~TKNXClient();
    int PreparePayload(uint8_t * buf, std::string payload);
    void HandlePacket(uint8_t * buf, int len);
    void Observe(PMQTTKnxObserver observer);
private:
    EIBConnection * out;
    EIBConnection * in;
    eibaddr_t src_addr;
    PMQTTKnxObserver Observer;
};
typedef std::shared_ptr<TKNXClient> PKNXClient;

#endif // KNX_CLIENT_H
