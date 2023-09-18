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
         * availability will only be available with Client provided.
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
         * @param client The MQTT client object.
        */
        HAMqttDevice(String device_name, EspMQTTClient& client);

        /**
         * @brief Add a custom config key value pair that will be used when
         * sending the config payload to MQTT. See available device config
         * here: https://www.home-assistant.io/integrations/sensor.mqtt/#device
         * Device configs are common whatever entity type (sensor, switch,
         * button, fan etc.) you are setting up.
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
         * @brief send an available payload at the device availabality topic.
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
        // see full list here: https://www.home-assistant.io/integrations/#search/mqtt
        enum Component{ALARM_CONTROL_PANEL, BINARY_SENSOR, BUTTON, CAMERA, COVER, DEVICE_TRACKER, DEVICE_TRIGGER, FAN, HUMIDIFIER, HVAC, LIGHT, LOCK, SIREN, SENSOR, SWITCH, VACUUM};

        /**
         * @brief Construct the entity object. Sending availability will only
         * be available if the device's Client is provided.
         *
         * @param device The device object.
         * @param name The entity name.
         * @param component What component this entity is, e.g.
         * HAMqttEntity::SENSOR. Available components are:
         * ALARM_CONTROL_PANEL, BINARY_SENSOR, BUTTON, CAMERA,
         * COVER, DEVICE_TRACKER, DEVICE_TRIGGER, FAN, HUMIDIFIER,
         * HVAC, LIGHT, LOCK, SIREN, SENSOR, SWITCH, VACUUM.
         * If you don't see yours in this list, create an issue
         * on my github https://github.com/MarcBresson/HA-MQTT.
        */
        HAMqttEntity(HAMqttDevice& device, String name, Component component);
        /**
         * @brief get entity name.
        */
        String getName();
        /**
         * @brief get entity identifier. It is constructed as follow:
         * [device identifier]-[entity name]
        */
        String getIdentifier();

        /**
         * @brief get entity base topic. It is constructed as
         * follow: homeassistant/[component name]/[device identifier].
        */
        String getBaseTopic();
        /**
         * @brief get entity availability topic. If relative is false,
         * will construct the topic with the base topic prepended. Otherwise,
         * it will prepend "~" that HA automatically interprets with the
         * base topic.
         * 
         * @param relative whether to include "~" or full base topic.
        */
        String getAvailabilityTopic(bool relative = false);
        /**
         * @brief get entity discovery topic. If relative is false,
         * will construct the topic with the base topic prepended. Otherwise,
         * it will prepend "~" that HA automatically interprets with the
         * base topic.
         * 
         * @param relative whether to include "~" or full base topic.
        */
        String getDiscoveryTopic(bool relative = false);
        /**
         * @brief get entity command topic. If relative is false,
         * will construct the topic with the base topic prepended. Otherwise,
         * it will prepend "~" that HA automatically interprets with the
         * base topic.
         * 
         * @param relative whether to include "~" or full base topic.
        */
        String getCommandTopic(bool relative = false);
        /**
         * @brief get entity state topic. If relative is false,
         * will construct the topic with the base topic prepended. Otherwise,
         * it will prepend "~" that HA automatically interprets with the
         * base topic.
         * 
         * @param relative whether to include "~" or full base topic.
        */
        String getStateTopic(bool relative = false);

        /**
         * @brief Add topic where home assistant will publish commands for the
         * entity. 
         * https://www.home-assistant.io/integrations/button.mqtt/#command_topic
        */
        void addCommandTopic();
        /**
         * @brief Add topic where this entity will report its state to home assistant. 
         * https://www.home-assistant.io/integrations/switch.mqtt/#state_topic
        */
        void addStateTopic();

        /**
         * @brief Add a custom config key value pair that will be used when
         * sending the config payload to MQTT. See available config for mqtt
         * sensors here:
         * https://www.home-assistant.io/integrations/sensor.mqtt/#configuration-variables
         *
         * @param key name of the config option.
         * @param value value of the config option.
        */
        void addConfig(const String &key, const String &value);
        /**
         * @brief Parse the config options of the entity and the device
         * into a string.
         *
         * @return serialised JSON config.
        */
        String getConfigPayload();

        /**
         * @brief send an available payload at the entity availabality topic.
        */
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
