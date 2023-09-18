#include <HAMqtt.h>

HAMqttEntity::HAMqttEntity(HAMqttDevice& device, String name, Component component){
    _device = &device;
    _name = name;
    _component = component;

    _identifier = _device->getIdentifier() + "-" + name;
    _identifier.replace(' ', '_');
    _identifier.toLowerCase();
}

String HAMqttEntity::getName(){
    return _name;
}
String HAMqttEntity::getIdentifier(){
    return _identifier;
}

void HAMqttEntity::addCommandTopic(){
    addConfig("cmd_t", _getTopic(true, "/set"));
}
void HAMqttEntity::addStateTopic(){
    addConfig("stat_t", _getTopic(true, "/state"));
}

String HAMqttEntity::getBaseTopic(){
    const String ha_topic = HA_TOPIC;
    return ha_topic + componentToStr(_component) + "/" + _device->getIdentifier();
}
String HAMqttEntity::getAvailabilityTopic(bool relative){
    return _getTopic(relative, + "/status");
}
String HAMqttEntity::getDiscoveryTopic(bool relative){
    // must conform to https://www.home-assistant.io/integrations/mqtt/#discovery-topic
    return _getTopic(relative, "/config");
}
String HAMqttEntity::getCommandTopic(bool relative){
    return _getTopic(relative, "/set");
}
String HAMqttEntity::getStateTopic(bool relative){
    return _getTopic(relative, "/state");
}
String HAMqttEntity::_getTopic(bool relative, String suffix){
    if(relative){
        return "~" + suffix;
    }
    return getBaseTopic() + suffix;
}

void HAMqttEntity::addConfig(const String &key, const String &value){
    _config.push_back({key, value});
}

String HAMqttEntity::getConfigPayload(){
    String s = "{";
    s += serializerKeyValue("name", _name);
    s += ",";
    s += serializerKeyValue("unique_id", getIdentifier());
    s += ",";
    s += serializerKeyValue("~", getBaseTopic());
    s += ",";
    s += serializerKeyValue("device", _device->getConfigPayload());
    s += ",";
    s += serializerDict(_config, false);
    s += ",";

    std::vector<String> availability;
    availability.push_back("{\"topic\":\"" + _device->getAvailabilityTopic() + "\"}");
    availability.push_back("{\"topic\":\"" + getAvailabilityTopic() + "\"}");
    s += serializerKeyValue("availability", serializerList(availability, true));

    s += "}";

    return s;
}

void HAMqttEntity::sendAvailable(){
    _device->getClient()->publish(getAvailabilityTopic(), "online");
}

String HAMqttEntity::componentToStr(Component component){
    // from https://www.home-assistant.io/integrations/#search/mqtt
    // update the list as it goes
    switch (component){
    case Component::ALARM_CONTROL_PANEL: return "alarm_control_panel";
    case Component::BINARY_SENSOR: return "binary_sensor";
    case Component::BUTTON: return "button";
    case Component::CAMERA: return "camera";
    case Component::COVER: return "cover";
    case Component::DEVICE_TRACKER: return "device_tracker";
    case Component::DEVICE_TRIGGER: return "device_trigger";
    case Component::FAN: return "fan";
    case Component::HUMIDIFIER: return "humidifier";
    case Component::HVAC: return "hvac";
    case Component::LIGHT: return "light";
    case Component::LOCK: return "lock";
    case Component::SIREN: return "siren";
    case Component::SENSOR: return "sensor";
    case Component::SWITCH: return "switch";
    case Component::VACUUM: return "vacuum";
    default: return "default";
    }
}
