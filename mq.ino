#include "mq.h"

// https://gitlab.com/painlessMesh/painlessMesh/-/blob/master/examples/mqttBridge/mqttBridge.ino
// prototypes
void mqttReceivedCallback( const uint32_t &from, const String &msg );
void mqttCallback(char* topic, byte* payload, unsigned int length);

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
IPAddress myIP(0, 0, 0, 0);

void mqttCallback(char* topic, uint8_t* payload, unsigned int length) {
  char* cleanPayload = (char*)malloc(length + 1);
  payload[length] = '\0';
  memcpy(cleanPayload, payload, length + 1);
  String msg = String(cleanPayload);

  String targetStr = String(topic).substring(24);
  Serial.println(targetStr);
  StaticJsonDocument<200> doc;

  DeserializationError error = deserializeJson(doc, msg);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  free(cleanPayload);

  String str = doc["str"];
  Serial.println(str);
}

String hash_id = sha1((String)ESP.getChipId());

void subscribe_after_connect() {
  String payload = "{\"id\":\"" + config.id + "\",\"code\":0,\"operation\":0,\"data\":\"logon request\"}";
  mqttClient.publish("meshNetwork/from/rootNode/logon", payload.c_str());
  mqttClient.subscribe("meshNetwork/to/rootNode/#");
}

void mqtt_init() {
  LogDivisionSta("MQTT INFO");
  
  if (config.wifi_mode == 1) {
    Serial.print("Logon MQTT ClientID: ");
    Serial.println(hash_id);
    mqttClient.setServer(config.mqtt_broker.c_str(), config.mqtt_port);
    mqttClient.setCallback(mqttCallback);
  }

  LogDivisionEnd("MQTT INFO");
}

long lastReconnectAttempt = 0;

boolean reconnect() {
  if (mqttClient.connect(hash_id.c_str(), config.mqtt_username.c_str(), config.mqtt_password.c_str())) {
    // Once connected, publish an announcement...
    subscribe_after_connect();
  }
  return mqttClient.connected();
}

void mqtt_update() {
  if (config.wifi_mode == 1 && myIP != getlocalIP()) {
    // not loopback address
    if (!mqttClient.connected()) {
      long now = millis();
      if (now - lastReconnectAttempt > 5000) {
        lastReconnectAttempt = now;
        if (reconnect()) {
          lastReconnectAttempt = 0;
        }
      }
    } else {
      mqttClient.loop();
    }
  }
}
