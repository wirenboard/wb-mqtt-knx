#include <string>
#include <memory>
#include <eibclient.h>
#ifndef KNX_CLIENT_H
#define KNX_CLIENT_H

class TKNXClient {
public:
    TKNXClient();
    void Send(std::string topic, std::string payload);
    void Loop();
private:
    EIBConnection * out;
    EIBConnection * in;
};
typedef std::shared_ptr<TKNXClient> PKNXClient;

#endif // KNX_CLIENT_H
