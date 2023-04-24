#include "ha_mqtt.h"
#include <ESP8266WiFi.h>

#define TOPIC_PREFIX "homeassistant/";
#define COMMAND_SUFFIX "/set";
#define STATE_SUFFIX "/state";
#define AVAILABILITY_SUFFIX "/status";
#define CONFIG_SUFFIX "/config";

void HA_MQTT_device::set_info(String device_name, String component, String software_id, String model_id, String manufacturer_id){
    set_wifi();

    _name = device_name;
    _identifier = _name + _mac_adress.substring(12);
    _identifier.replace(' ', '_');
    _identifier.toLowerCase();

    _software_id = software_id;
    _model_id = model_id;
    _manufacturer_id = manufacturer_id;

    // must conform to https://www.home-assistant.io/integrations/mqtt/#discovery-topic
    const String topic_prefix = TOPIC_PREFIX;
    _topic = topic_prefix + component + "/" + _identifier;
}

void HA_MQTT_device::set_client(PubSubClient& client, String mqtt_username, String mqtt_password){
    _client = &client;
    _mqtt_username = mqtt_username;
    _mqtt_password = mqtt_password;
}

void HA_MQTT_device::set_wifi(){
    _mac_adress = WiFi.macAddress();
    _mac_adress.replace(":", "-");
    _ip_adress = WiFi.localIP().toString();
}

boolean HA_MQTT_device::connect(){
    return _client->connect(
            _identifier.c_str(),
            _mqtt_username.c_str(),
            _mqtt_password.c_str()
        );
}

void HA_MQTT_device::wait_for_connection(uint32_t retry_delay){
    while (!_client->connected()){
        if(connect()){
            Serial.println("Mqtt broker connected.");
        } else {
            Serial.print("Mqtt broker not connected. state = ");
            Serial.println(_client->state());
            wait_for_connection(retry_delay);
        };
    }
}

void HA_MQTT_device::send_discovery(char* conf_buffer, const char* discovery_topic){
    if (_client->publish(discovery_topic, conf_buffer, true)){
        Serial.println("... configuration sent");
    } else {
        Serial.println("... failed to send configuration ...");
        delay(1000);
    }       
}




HA_MQTT_entity::HA_MQTT_entity(HA_MQTT_device& device){
    _device = &device;
}

void HA_MQTT_entity::auto_discovery(String entity_name, String entity_id){
    const String availability_suffix = AVAILABILITY_SUFFIX;
    const String state_suffix = STATE_SUFFIX;
    const String command_suffix = COMMAND_SUFFIX;

    _name = entity_name;
    _id = entity_id;

    _conf["name"] = entity_name;
    _conf["unique_id"] = _device->_manufacturer_id + "-" + _device->_mac_adress + "-" + _id;

    _conf["~"] = getBaseTopic();

    _conf["availability_topic"] = "~" + availability_suffix;

    _conf["stat_t"] = "~" + state_suffix;
    _conf["cmd_t"] = "~" + command_suffix;
    
    JsonObject device = _conf.createNestedObject("device");
    device["name"] = _device->_name;
    device["sw_version"] = _device->_software_id;
    device["model"] = _device->_model_id;
    device["manufacturer"] = _device->_manufacturer_id;

    JsonArray identifiers = device.createNestedArray("identifiers");
    identifiers.add(_device->_mac_adress);

    send_discovery();
}

void HA_MQTT_entity::send_discovery(){
    char buffer[600];
    serializeJsonPretty(_conf, buffer);

    Serial.println(buffer);

    if (_device->_client->publish(getDiscoveryTopic().c_str(), buffer, true)){
        Serial.println("... configuration sent");
    } else {
        Serial.println("... failed to send configuration ...");
        delay(4000);
        send_discovery();
    }   
}

void HA_MQTT_entity::send_available(){
    String availability_suffix = AVAILABILITY_SUFFIX;
    _device->_client->publish(getAvailabilityTopic().c_str(), "online", false);
}

String HA_MQTT_entity::getBaseTopic(){
    return _device->_topic + _id;
}
String HA_MQTT_entity::getAvailabilityTopic(){
    String availability_suffix = AVAILABILITY_SUFFIX;
    return (getBaseTopic() + availability_suffix);
}
String HA_MQTT_entity::getDiscoveryTopic(){
    String CONFIG_SUFFIX = CONFIG_SUFFIX;
    return (getBaseTopic() + CONFIG_SUFFIX);
}
String HA_MQTT_entity::getCommandTopic(){
    String command_suffix = COMMAND_SUFFIX;
    return (getBaseTopic() + command_suffix);
}
String HA_MQTT_entity::getStateTopic(){
    String state_suffix = STATE_SUFFIX;
    return (getBaseTopic() + state_suffix);
}
