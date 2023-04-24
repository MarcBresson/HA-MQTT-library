#ifndef class_HA_MQTT_device
#define class_HA_MQTT_device

#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <stdlib.h>

class HA_MQTT_device {
    public:
        boolean connect();
        void wait_for_connection(uint32_t retry_delay = 3000);
        void set_info(String device_name, String component, String software_id, String model_id, String manufacturer_id);
        void set_client(PubSubClient& client, String mqtt_username, String mqtt_password);
        void set_wifi();
        void send_discovery(char* conf_buffer, const char* discovery_topic);
        
        PubSubClient* _client;
        String _mqtt_username;
        String _mqtt_password;

        String _name;
        String _identifier;
        String _software_id;
        String _model_id;
        String _manufacturer_id;

        String _topic;

        String _mac_adress;
        String _ip_adress;
};

class HA_MQTT_entity {
    public:
        explicit HA_MQTT_entity(HA_MQTT_device& device);
        void send_available();
        void auto_discovery(String entity_name, String entity_id);
        String getBaseTopic();
        String getAvailabilityTopic();
        String getDiscoveryTopic();
        String getCommandTopic();
        String getStateTopic();

        HA_MQTT_device* _device;

        String _name;
        String _id;
    private:
        StaticJsonDocument<512> _conf;

        void send_discovery();
};
#endif
