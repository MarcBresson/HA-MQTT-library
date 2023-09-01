#ifndef class_HA_MQTT_device
#define class_HA_MQTT_device

#define HA_TOPIC "homeassistant/";

#ifdef ESP8266
  #include <ESP8266WiFi.h>
#else // for ESP32
  #include <WiFiClient.h>
#endif

#include "EspMQTTClient.h"
#include <ArduinoJson.h>
#include <vector>

struct Dict{
    String key;
    String value;
};
String serializerDict(std::vector<Dict> dictionnary, bool enclose);
String serializerList(std::vector<String> array, bool enclose);
String serializerKeyValue(String key, String value);

class HAMqttDevice {
    public:
        HAMqttDevice(String device_name, EspMQTTClient& client);

        void addConfig(const String &key, const String &value);
        String getConfigPayload();

        String getName();
        String getIdentifier();
        String getAvailabilityTopic();

        void manageAvailability(uint16_t keepAliveSecond);
        void sendAvailable();

    private:
        EspMQTTClient* _client;
        String _name;
        String _identifier;
        String _mac_adress;

        long lastAvailabilityMillis = 0;

        std::vector<Dict> _config;
};

class HAMqttEntity {
    public:
        enum Component{ALARM_CONTROL_PANEL, BINARY_SENSOR, BUTTON, CAMERA, COVER, DEVICE_TRACKER, DEVICE_TRIGGER, FAN, HUMIDIFIER, HVAC, LIGHT, LOCK, SIREN, SENSOR, SWITCH, VACUUM};

        HAMqttEntity(EspMQTTClient& client, HAMqttDevice& device, String name, Component component);
        String getName();
        String getIdentifier();

        String getBaseTopic();
        String getAvailabilityTopic(bool relative = false);
        String getDiscoveryTopic(bool relative = false);
        String getCommandTopic(bool relative = false);
        String getStateTopic(bool relative = false);

        void addCommandTopic();
        void addStateTopic();

        void addConfig(const String &key, const String &value);
        String getConfigPayload();

        void sendAvailable();

    private:
        EspMQTTClient* _client;
        HAMqttDevice* _device;

        String _name;
        String _identifier;

        std::vector<Dict> _config;

        String _getTopic(bool relative, String suffix);

        Component _component;
        static String componentToStr(Component component);
};
#endif
