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
        /**
         * @brief Construct the device object. Managing or sending
         * availability will not be available with Client provided.
         *
         * @param device_name The name of your device. It should not contains
         * accentuated letters.
        */
        HAMqttDevice(String device_name);
        /**
         * @brief Construct the device with the Client object. It allows
         * the use of manageAvailability and sendAvailable methods.
         *
         * @param device_name The name of your device. It should not contains
         * accentuated letters.
         * @param client The client object.
        */
        HAMqttDevice(String device_name, EspMQTTClient& client);

        /**
         * @brief Add a custom config key value pair that will be used when
         * sending the config payload to MQTT. See available config for mqtt
         * sensors here:
         * https://www.home-assistant.io/integrations/sensor.mqtt/#device
         *
         * @param key name of the config option.
         * @param value value of the config option.
        */
        void addConfig(const String &key, const String &value);
        /**
         * @brief Parse the config options into a string.
         *
         * @return serialised JSON config.
        */
        String getConfigPayload();

        /**
         * @brief get device name.
        */
        String getName();
        /**
         * @brief get device identifier. It is constructed as follow:
         * [device_name]-[last MAC adress characters]
        */
        String getIdentifier();
        /**
         * @brief get device availability topic. It is constructed as
         * follow: homeassistant/[device identifier]/status.
         * 
         * You can change the "homeassistant/" discovery topic by
         * defining HA_TOPIC and chaning the value is HA.
         * https://www.home-assistant.io/integrations/mqtt/#discovery-topic
         * 
        */
        String getAvailabilityTopic();

        /**
         * @brief will send an available payload every n seconds.
         * 
         * @param keepAliveSecond number of seconds to wait before sending
         * a new available message.
        */
        void manageAvailability(uint16_t keepAliveSecond);
        /**
         * @brief send an available payload.
        */
        void sendAvailable();

        EspMQTTClient* getClient();

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

        HAMqttEntity(HAMqttDevice& device, String name, Component component);
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
