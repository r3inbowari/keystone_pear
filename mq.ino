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
  Serial.println("[MQTT CALLBACK] topic: " + targetStr);
  StaticJsonDocument<300> doc;

  DeserializationError error = deserializeJson(doc, msg);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  free(cleanPayload);

  int operation = doc["operation"];
  String id = doc["id"];
  int member = doc["member"];

  Serial.println("[MQTT CALLBACK] receive operation: " + String(operation));
  if (targetStr == "led") {
    if (operation == OP::LEDTurnOff) {
      digitalWrite(2, LOW);
    } else if (operation == OP::LEDTurnOff) {
      digitalWrite(2, HIGH);
    }
  } else if (targetStr == "checkupdate") {
    int remote_major = doc["major"];
    int remote_minor = doc["minor"];
    int remote_patch = doc["patch"];
    setRemoteVersion(remote_major, remote_minor, remote_patch);
    if (id == config.id) {
      check_update();
    } else if (id == "null" ) {
      check_update_not_update();
    }
  } else if (targetStr == "coupler") {
    if (id == config.id) {
      if (operation == OP::CouplerTurnOff) {
        turn_off_coupler(member);
      } else if (operation == OP::CouplerTurnOn) {
        turn_on_coupler(member);
      }
    } else {
      auto ts = now();
      if (operation == OP::CouplerTurnOff) {
        auto payload = "{\"id\":\"" + id + "\",\"mid\":\"" + mesh.getNodeId() + "\",\"ts\":" + now() + ",\"operation\":" + OP::CouplerTurnOff + ",\"data\":\"" + String(member) + "\"}";
        Serial.println(payload);
        mesh.sendBroadcast(payload, false);
      } else if (operation == OP::CouplerTurnOn) {
        auto payload = "{\"id\":\"" + id + "\",\"mid\":\"" + mesh.getNodeId() + "\",\"ts\":" + now() + ",\"operation\":" + OP::CouplerTurnOn + ",\"data\":\"" + String(member) + "\"}";
        Serial.println(payload);
        mesh.sendBroadcast(payload, false);
      }
    }
  }
}

String hash_id = sha1((String)ESP.getChipId());

void subscribe_after_connect() {
  // String payload = "{\"id\":\"" + config.id + "\",\"code\":0,\"operation\":" + OP::LogonRequest + ",\"data\":\"logon request\"}";
  String payload = "{\"id\":\"" + config.id + "\",\"code\":0,\"operation\":12,\"data\":\"heartbeat\",\"ssid\":\"" + config.ssid + "\",\"prefix\":\"" + config.mesh_prefix + "\",\"mode\":" + config.wifi_mode + ",\"broker\":\"" + config.wifi_mode + "\",\"username\":\"" + config.mqtt_username + "\",\"major\":" + config.major + ",\"minor\":" + config.minor + ",\"patch\":" + config.patch + ",\"ts\":" + now() + "}";

  mqttClient.publish("meshNetwork/from/rootNode/logon", payload.c_str());
  mqttClient.subscribe("meshNetwork/to/rootNode/#");
  update_check();
}

void update_request() {
  String payload = "{\"id\":\"" + config.id + "\",\"operation\":3,\"member\":0}";
  mqttClient.publish("meshNetwork/from/rootNode/checkupdate", payload.c_str());
}

void update_check() {
  String payload = "{\"id\":\"null\",\"operation\":3,\"member\":0}";
  mqttClient.publish("meshNetwork/from/rootNode/checkupdate", payload.c_str());
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
