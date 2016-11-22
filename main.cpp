#include <iostream>
#include <wbmqtt/mqtt_wrapper.h>
#include <getopt.h>
#include <unistd.h>
#include <eibclient.h>
using namespace std;

bool debug;

class TMQTTKnxObserver : public IMQTTObserver, public std::enable_shared_from_this<TMQTTKnxObserver> {
public:
    TMQTTKnxObserver(PMQTTClientBase mqtt_client);
    void SetUp();
    void OnConnect(int rc);
    void OnMessage(const mosquitto_message *message);
    void OnSubscribe(int mid, int qos_count, const int *granted_qos);

    void LoopOnce();
    void Loop();
private:
    PMQTTClientBase MQTTClient;
};

TMQTTKnxObserver::TMQTTKnxObserver(PMQTTClientBase mqtt_client) : MQTTClient(mqtt_client) {}

void TMQTTKnxObserver::SetUp() {
    MQTTClient->Observe(shared_from_this());
    MQTTClient->Connect();
}

void TMQTTKnxObserver::OnConnect(int rc) {
    if(debug) cout << "KNX observer on connect\n";
    MQTTClient->Subscribe(NULL, "/devices/knx/#");
}

void TMQTTKnxObserver::OnMessage(const mosquitto_message *message) {
    std::string topic = message->topic;
    std::string payload = static_cast<const char *>(message->payload);
    if(debug) cout << "KNX observer topic: " << topic << " message: " << payload << "\n";
    //EIBConnection * conn = EIBSocketRemote("localhost", 6720);
    EIBConnection * conn = EIBSocketLocal("/tmp/eib");
    if(debug) cout << (conn != NULL) << "\n";
    eibaddr_t addr = 2309;
    uint8_t val[] = {'t','r','u','e', 0};
    int res = EIBOpenT_Group(conn, addr, 0);
    if(debug) cout << res << "\n";

    res = EIBSendAPDU(conn, 5, val);
    if(debug) cout << res << "\n";
    res = EIBComplete(conn);
    if(debug) cout << res << "\n";
    res = EIBClose(conn);
    if(debug) cout << res << "\n";
}
void TMQTTKnxObserver::Loop(){
    sleep(10);
}
void TMQTTKnxObserver::OnSubscribe(int mid, int qos_count, const int *granted_qos) {
    if(debug) cout << "KNS subscription succeeded\n";
}

typedef shared_ptr<TMQTTKnxObserver> PMQTTKnxObserver;

int main(int argc, char ** argv)
{
    debug = true;
    TMQTTClient::TConfig mqtt_config;
    mqtt_config.Host = "localhost";
    mqtt_config.Port = 1883;
    int c;
    while ((c = getopt(argc, argv, "dp:h:"))!=-1) {
        switch (c){
        case 'd':
            debug = true;
            break;
        case 'p':
            mqtt_config.Port = stoi(optarg);
            break;
        case 'h':
            mqtt_config.Host = optarg;
            break;
        default:
            break;
        }
    }
    mosqpp::lib_init();
    mqtt_config.Id = "wb-knx";

    PMQTTClient mqtt_client(new TMQTTClient(mqtt_config));
    try {
        PMQTTKnxObserver observer(new TMQTTKnxObserver(mqtt_client));
        observer->SetUp();
        mqtt_client->StartLoop();
        observer->Loop();
    } catch(...){
        cout << "exception found\n";
    }

    return 0;
}

