#include <iostream>
#include <wbmqtt/mqtt_wrapper.h>
#include <getopt.h>
#include <unistd.h>
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

}
void TMQTTKnxObserver::Loop(){
    sleep(10);
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

