# HA MQTT discovery

This library handles devices and entities to enable auto-discovery of MQTT devices in Home Assistant. It works with EspMQTTClient from https://registry.platformio.org/libraries/plapointe6/EspMQTTClient.

# API

## TOC

- [Device](#device)
  - [Constructor](#constructor)
  - [Custom Config Variables](#custom-config-variables)
  - [Manage availability](#manage-availability)
  - [Other methods](#other-methods)
- [Entity](#entity)
  - [Constructor](#constructor-1)
  - [Add Command topic](#add-command-topic)
  - [Add State topic](#add-state-topic)
  - [Custom Config Variables](#custom-config-variables-1)
  - [Get State topic](#get-state-topic)
  - [Other methods](#other-methods-1)
- [Examples](#examples)

## Device

### Constructor

```cpp
HAMqttDevice(String device_name, (optional) EspMQTTClient& client);
```

Construct the device object. Managing or sending availability will only work if Client provided.

#### Parameters

- **device_name** The name of your device. It should not contains accentuated letters.
- **client** The MQTT client object.

#### Example

```cpp
EspMQTTClient client(
  WIFI_SSID,
  WIFI_PASSWORD,
  MQTT_IP,
  MQTT_USERNAME,
  MQTT_PASSWORD
);

void addConfig(const String &key, const String &value);
```

### Custom config variables

```cpp
void addConfig(const String &key, const String &value);
```

Add a custom config key value pair that will be used when sending the config payload to MQTT. See available device config here: https://www.home-assistant.io/integrations/sensor.mqtt/#device. Device configs are common whatever entity type (sensor, switch, button, fan etc.) you are setting up.

#### Parameters

- **key** name of the config option.
- **value** value of the config option.

#### Example

```cpp
device.addConfig("sw_version", "0.0.1");
device.addConfig("manufacturer", "Marc Bresson");
```

### Manage availability

```cpp
void manageAvailability(uint16_t keepAliveSecond);
```

will send an available payload every n seconds.

#### Parameters

- **keepAliveSecond** number of seconds to wait before sending a new available message.

#### Example

```cpp
void loop() {
    client.loop();

    // wait 60 seconds between availability message.
    device.manageAvailability(60);
}
```

### Other methods

Their documentations are available in the header file.

```cpp
String getConfigPayload();
String getName();
String getIdentifier();
String getAvailabilityTopic();
void sendAvailable();
EspMQTTClient* getClient();
```

## Entity

### Constructor

```cpp
HAMqttEntity(HAMqttDevice& device, String name, Component component);
```

Construct the entity object. Sending availability will only be available if the device's Client is provided.

#### Parameters

- **device** The device object.
- **name** The entity name.
- **component** What component this entity is, e.g. `HAMqttEntity::SENSOR`. Available components are: `ALARM_CONTROL_PANEL`, `BINARY_SENSOR`, `BUTTON`, `CAMERA`, `COVER`, `DEVICE_TRACKER`, `DEVICE_TRIGGER`, `FAN`, `HUMIDIFIER`, `HVAC`, `LIGHT`, `LOCK`, `SIREN`, `SENSOR`, `SWITCH`, `VACUUM`. If you don't see yours in this list, create an issue on my github https://github.com/MarcBresson/HA-MQTT.

#### Example

```cpp
HAMqttEntity entityEnergy(device, "Grid Energy", HAMqttEntity::SENSOR);
```

### Add command topic

```cpp
void addCommandTopic();
```

Add topic where home assistant will publish commands for the entity. (see more https://www.home-assistant.io/integrations/button.mqtt/#command_topic).

#### Example

```cpp
entitySwitch.addCommandTopic();
```

### Add state topic

```cpp
void addStateTopic();
```

Add topic where this entity will report its state to home assistant. . (see more https://www.home-assistant.io/integrations/switch.mqtt/#state_topic).

#### Example

```cpp
entityEnergy.addStateTopic();
```

### Custom config variables

```cpp
void addConfig(const String &key, const String &value);
```

Add a custom config key value pair that will be used when sending the config payload to MQTT. See available config for mqtt sensors here: https://www.home-assistant.io/integrations/sensor.mqtt/#configuration-variables

#### Parameters

- **key** name of the config option.
- **value** value of the config option.

#### Example

```cpp
entityEnergy.addConfig("device_class", "energy");
entityEnergy.addConfig("state_class", "total_increasing");
entityEnergy.addConfig("unit_of_measurement", "Wh");
```

### Get state topic

```cpp
void getStateTopic();
```

get entity state topic. If relative is false, will construct the topic with the base topic prepended. Otherwise, it will prepend "~" that HA automatically interprets with the base topic.

#### Parameters

- **relative** whether to include "~" or full base topic.

#### Example

```cpp
client.publish(entityEnergy.getStateTopic(), String(energyCount));
```

### Other methods

Their documentations are available in the header file.

```cpp
String getName();
String getIdentifier();
String getBaseTopic();
String getAvailabilityTopic(bool relative = false);
String getDiscoveryTopic(bool relative = false);
String getCommandTopic(bool relative = false);
String getConfigPayload();
void sendAvailable();
```

## Examples

- [Energy Meter](examples/energy_metter.cpp) as a sensor
- [RBG Lamp](examples/rgb_lamp.cpp) as a Light
