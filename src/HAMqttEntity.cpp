#include <HAMqtt.h>

HAMqttEntity::HAMqttEntity(){}

HAMqttEntity::HAMqttEntity(HAMqttDevice& device, String name, Component component){
    setDevice(device);
    setName(name);
    setComponent(component);

    init();
}

void HAMqttEntity::setDevice(HAMqttDevice& device){
    _device = &device;
}

void HAMqttEntity::setName(String name){
    _name = name;
}

void HAMqttEntity::setComponent(Component component){
    _component = component;
}

void HAMqttEntity::init(){
    _identifier = _device->getIdentifier() + "-" + _name;
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
    addConfig("cmd_t", getTopic(true, "/set"));
}
void HAMqttEntity::addStateTopic(){
    addConfig("stat_t", getTopic(true, "/state"));
}

String HAMqttEntity::getBaseTopic(){
    const String ha_topic = HA_TOPIC;
    return ha_topic + componentToStr(_component) + "/" + _identifier;
}
String HAMqttEntity::getAvailabilityTopic(bool relative){
    return getTopic(relative, + "/status");
}
String HAMqttEntity::getDiscoveryTopic(bool relative){
    // must conform to https://www.home-assistant.io/integrations/mqtt/#discovery-topic
    return getTopic(relative, "/config");
}
String HAMqttEntity::getCommandTopic(bool relative){
    return getTopic(relative, "/set");
}
String HAMqttEntity::getStateTopic(bool relative){
    return getTopic(relative, "/state");
}
String HAMqttEntity::getTopic(bool relative, String suffix){
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

EspMQTTClient* HAMqttEntity::getClient(){
    return _device->getClient();
}

HAMqttDevice* HAMqttEntity::getDevice(){
    return _device;
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
