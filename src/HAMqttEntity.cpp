#include "ha_mqtt.h"

HA_MQTT_entity::HA_MQTT_entity(HAMqttDevice& device){
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
