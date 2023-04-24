#include "ha_mqtt.h"

void HAMqttDevice::set_info(String device_name, String component, String software_id, String model_id, String manufacturer_id){
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

void HAMqttDevice::set_client(PubSubClient& client, String mqtt_username, String mqtt_password){
    _client = &client;
    _mqtt_username = mqtt_username;
    _mqtt_password = mqtt_password;
}

void HAMqttDevice::set_wifi(){
    _mac_adress = WiFi.macAddress();
    _mac_adress.replace(":", "-");
    _ip_adress = WiFi.localIP().toString();
}

boolean HAMqttDevice::connect(){
    return _client->connect(
            _identifier.c_str(),
            _mqtt_username.c_str(),
            _mqtt_password.c_str()
        );
}

void HAMqttDevice::wait_for_connection(uint32_t retry_delay){
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

void HAMqttDevice::send_discovery(char* conf_buffer, const char* discovery_topic){
    if (_client->publish(discovery_topic, conf_buffer, true)){
        Serial.println("... configuration sent");
    } else {
        Serial.println("... failed to send configuration ...");
        delay(1000);
    }       
}

String HAMqttDevice::deviceTypeToStr(DeviceType type)
{
    switch (type)
    {
    case DeviceType::ALARM_CONTROL_PANEL:
        return "alarm_control_panel";
    case DeviceType::BINARY_SENSOR:
        return "binary_sensor";
    case DeviceType::CAMERA:
        return "camera";
    case DeviceType::COVER:
        return "cover";
    case DeviceType::FAN:
        return "fan";
    case DeviceType::LIGHT:
        return "light";
    case DeviceType::LOCK:
        return "lock";
    case DeviceType::SENSOR:
        return "sensor";
    case DeviceType::SWITCH:
        return "switch";
    case DeviceType::CLIMATE:
        return "climate";
    case DeviceType::VACUUM:
        return "vacuum";
    case DeviceType::NUMBER:
        return "number";
    case DeviceType::BUTTON:
        return "button";
    default:
        return "[Unknown DeviceType]";
    }
}
