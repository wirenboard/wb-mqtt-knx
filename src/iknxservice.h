
#ifndef WB_MQTT_KNX_IKNXSERVICE_H
#define WB_MQTT_KNX_IKNXSERVICE_H

namespace knx
{
    class IKnxService
    {
    public:
        virtual void Start() = 0;
        virtual void Stop() = 0;

        virtual ~IKnxService() = default;
    };
}

#endif // WB_MQTT_KNX_IKNXSERVICE_H
