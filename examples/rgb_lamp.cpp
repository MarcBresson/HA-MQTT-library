#include "EspMQTTClient.h"
#include "FastLED.h"
#include "HAMqtt.h"
#include <Credentials.h>

#define MQTT_KEEPALIVE 30

EspMQTTClient client(
  WIFI_SSID,
  WIFI_PASSWORD,
  MQTT_IP,
  MQTT_USERNAME,
  MQTT_PASSWORD
);

HAMqttDevice rgbLampDevice("My RGB Lamp", client);

HAMqttEntity rgbLamp(rgbLampDevice, "My RGB Lamp", HAMqttEntity::LIGHT);

#define DATA_PIN    5
#define NUM_LEDS    77
CRGB leds[NUM_LEDS];

// colour and transition handling
uint8_t initialRed = 255;
uint8_t initialGreen = 255;
uint8_t initialBlue = 255;
uint8_t initialBrightness = 255;
uint8_t red = 255;
uint8_t green = 255;
uint8_t blue = 255;
uint8_t brightness = 255;
uint8_t targetRed = 255;
uint8_t targetGreen = 255;
uint8_t targetBlue = 255;
uint8_t targetBrightness = 255;
uint8_t targetSetBrightness = 255;

long transitionMillis = 0;
long transitionTime = 400;
bool transitionFinish = true;

bool stateOn = false;


void setup() {
  Serial.begin(9600);
  rgbLampDevice.addConfig("sw_version", "0.0.1");
  rgbLampDevice.addConfig("manufacturer", "Marckle");

  client.setKeepAlive(MQTT_KEEPALIVE);
  client.setMaxPacketSize(1024);
  client.enableDebuggingMessages();

  rgbLamp.addCommandTopic();
  rgbLamp.addStateTopic();
  rgbLamp.addConfig("brightness", "true");
  rgbLamp.addConfig("color_mode", "true");
  rgbLamp.addConfig("supported_color_modes", "[\"rgb\"]");
  rgbLamp.addConfig("schema", "json");

  pinMode(DATA_PIN, OUTPUT);
  FastLED.addLeds<WS2811, DATA_PIN, BRG>(leds, NUM_LEDS);
}




void callback(const String &message);
bool processJson(const String message);
void setColor(int inR, int inG, int inB, int inBrightness);
void updateState();


void onConnectionEstablished(){
  client.publish(rgbLamp.getDiscoveryTopic(), rgbLamp.getConfigPayload());
  client.subscribe(rgbLamp.getCommandTopic(), callback);
  updateState();
}




void callback(const String &message) {
  if (!processJson(message)) {
    return;
  }
}




bool processJson(const String message) {
  StaticJsonDocument<128> root;

  DeserializationError err = deserializeJson(root, message);

  if (err) {
    Serial.print("deserializeJson() failed with code ");
    Serial.println(err.f_str());
    return false;
  }

  if (root.containsKey("state")) {
    if (strcmp(root["state"], "ON") == 0) {
      stateOn = true;
      targetBrightness = targetSetBrightness;
      initialBrightness = brightness;
    }
    else if (strcmp(root["state"], "OFF") == 0) {
      stateOn = false;
      targetBrightness = 0;
      initialBrightness = brightness;
    }
  }

  if (root.containsKey("color")) {
    targetRed = root["color"]["r"];
    targetGreen = root["color"]["g"];
    targetBlue = root["color"]["b"];
    initialRed = red;
    initialGreen = green;
    initialBlue = blue;
  }

  if (root.containsKey("brightness")) {
    targetSetBrightness = root["brightness"];
    targetBrightness = targetSetBrightness;
    initialBrightness = brightness;
  }

  transitionMillis = millis();
  transitionFinish = false;
  if (root.containsKey("transition")) {
    transitionTime = root["transition"];
  }
  else {
    transitionTime = 400;
  }

  return true;
}

void setColor(uint8_t inR, uint8_t inG, uint8_t inB, uint8_t inBrightness) {
  red = inR;
  green = inG;
  blue = inB;
  brightness = inBrightness;

  Serial.print(red);
  Serial.print(",");
  Serial.print(green);
  Serial.print(",");
  Serial.print(blue);
  Serial.print(",");
  Serial.print(brightness);
  Serial.println("");

  for (int ii = 0; ii < NUM_LEDS; ii++){
    leds[ii] = CRGB(red, green, blue);
  }
  FastLED.setBrightness(brightness);
  FastLED.show();
}

void transitionEngine(){
  uint8 transitionProgress = (millis() - transitionMillis) * 100 / transitionTime;

  if (transitionFinish == false){
    if(transitionProgress > 100){
      transitionProgress = 100;
    }

    uint8_t tr_r = initialRed + ((targetRed - initialRed) * transitionProgress) / 100;
    uint8_t tr_g = initialGreen + ((targetGreen - initialGreen) * transitionProgress) / 100;
    uint8_t tr_b = initialBlue + ((targetBlue - initialBlue) * transitionProgress) / 100;
    uint8_t tr_v = initialBrightness + ((targetBrightness - initialBrightness) * transitionProgress) / 100;

    setColor(tr_r, tr_g, tr_b, tr_v);
    
    if(transitionProgress == 100){
      transitionFinish = true;
      updateState();
    }
  }
}

void updateState(){
  String stateOnOff = (stateOn) ? "ON" : "OFF";
  String state = "{\"state\":\"" + stateOnOff + "\",\"brightness\":" + brightness + ",\"color\":{\"r\":" + red + ",\"g\":" + green + ",\"b\":" + blue + "}}";
  client.publish(rgbLamp.getStateTopic(), state);
}

void loop() {
  rgbLampDevice.manageAvailability(MQTT_KEEPALIVE);
  client.loop();

  transitionEngine();
}
