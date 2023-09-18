#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <HAMqtt.h>
#include "Credentials.h"
#include "PulseCounter.h"

#define MQTT_KEEPALIVE 60

EspMQTTClient client(
  WIFI_SSID,
  WIFI_PASSWORD,
  MQTT_IP,
  MQTT_USERNAME,
  MQTT_PASSWORD
);

// the device needs the client if the availability is handled device-wise. Instead of
// sending one message per entity for availability, you send one message for the device
// availability.
HAMqttDevice energyMeter("Energy Meter", client);

// the entity needs the device to get identifiers, topics or the config payload.
HAMqttEntity EnergyConsumed(energyMeter, "Energy consumed", HAMqttEntity::SENSOR);

PulseListener PulseCounter(D1, 10000);

void setup() {
  Serial.begin(9600);
  // adds device config variables.
  energyMeter.addConfig("sw_version", "0.0.1");
  energyMeter.addConfig("manufacturer", "Marc Bresson");

  client.setKeepAlive(MQTT_KEEPALIVE);
  client.setMaxPacketSize(1024);
  // prints info on Serial everytime it tries to connect, send or receive a payload.
  client.enableDebuggingMessages();

  // enable the entity to report its state to home assistant.
  EnergyConsumed.addStateTopic();
  // adds custom config variables for the entity
  EnergyConsumed.addConfig("device_class", "energy");
  EnergyConsumed.addConfig("state_class", "total_increasing");
  EnergyConsumed.addConfig("unit_of_measurement", "Wh");
}

void onConnectionEstablished(){
  // sends the configuration payload on the configuration topic.
  client.publish(EnergyConsumed.getDiscoveryTopic(), EnergyConsumed.getConfigPayload(), true);
}

uint16_t previous_pulse_count = 0;
void loop() {
    client.loop();

    // auto sends status message twice in the keepAlive interval.
    energyMeter.manageAvailability(MQTT_KEEPALIVE);

    PulseCounter.update();

    if (PulseCounter.isNewPulseReady(previous_pulse_count)){
        previous_pulse_count = PulseCounter.getPulseCount();
        PulseCounter.resetTimer();

        client.publish(EnergyConsumed.getStateTopic(), String(PulseCounter.getPulseCount()));
    }
}
