#include "conf.h"

void loadConfiguration(const char *filename, Config &config);

const char *filename = "/config.json";  // <- Flash library uses 8.3 filenames
Config config;                          // <- global configuration object

// Loads the configuration from a filenames
void loadConfiguration(const char *filename, Config &config) {
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    return;
  }
  File file = LittleFS.open(filename, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
  }

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<1024> doc;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error)
    Serial.println(F("Failed to read file, using default configuration"));

  // Copy values from the JsonDocument to the Config
  config.id = doc["id"] | "null";
  config.type = doc["type"] | "null";
  config.major = doc["major"] | 0;
  config.minor = doc["minor"] | 0;
  config.patch = doc["patch"] | 0;
  config.ssid = doc["ssid"] | "";
  config.password = doc["password"] | "";
  config.update_host = doc["update_host"] | "null";
  config.tcp_host = doc["tcp_host"] | "null";

  config.mesh_prefix = doc["mesh_prefix"] | "null";
  config.mesh_password = doc["mesh_password"] | "null";
  config.mesh_port = doc["mesh_port"] | 5555;

  config.wifi_mode = doc["wifi_mode"] | 0;

  config.mqtt_broker = doc["mqtt_broker"] | "null";
  config.mqtt_port = doc["mqtt_port"] | 1883;
  config.mqtt_username = doc["mqtt_username"] | "null";
  config.mqtt_password = doc["mqtt_password"] | "null";
  //  strlcpy(config.type,          // <- destination
  //  doc["type"] | "example.com",  // <- source
  //  sizeof(config.type));         // <- destination's capacity
  LittleFS.end();
}

// Saves the configuration to a file
void saveConfiguration(const char *filename, const Config &config) {
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    return;
  }
  // Delete existing file, otherwise the configuration is appended to the file
  if (LittleFS.remove(filename)) {
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }

  // Open file for writing
  File file = LittleFS.open(filename, "w");
  if (!file) {
    Serial.println(F("Failed to create file"));
    return;
  }

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/assistant to compute the capacity.
  StaticJsonDocument<1024> doc;

  // Set the values in the document
  doc["id"] = config.id;
  doc["type"] = config.type;
  doc["major"] = config.major;
  doc["minor"] = config.minor;
  doc["patch"] = config.patch;
  doc["ssid"] = config.ssid;
  doc["password"] = config.password;
  doc["update_host"] = config.update_host;
  doc["tcp_host"] = config.tcp_host;

  doc["mesh_prefix"] = config.mesh_prefix;
  doc["mesh_password"] = config.mesh_password;
  doc["mesh_port"] = config.mesh_port;
  
  doc["wifi_mode"] = config.wifi_mode;

  doc["mqtt_broker"] = config.mqtt_broker;
  doc["mesh_port"] = config.mesh_port;
  doc["mqtt_username"] = config.mqtt_username;
  doc["mqtt_password"] = config.mqtt_password;
  // Serialize JSON to file
  if (serializeJson(doc, file) == 0) {
    Serial.println(F("Failed to write to file"));
  }

  // Close the file
  file.close();
}

/**
   设置Config
*/
bool setConfig(const char * config_str) {
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    return false;
  }
  writeFile("/config.json", config_str);
  LittleFS.end();
  return true;
}

/**
   获取Config
*/
JsonObject * getConfig() {
  DynamicJsonDocument doc(1024);
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    return NULL;
  }
  char ret2[1024] = { 0 };
  readFile("/config.json", ret2);
  deserializeJson(doc, ret2);
  JsonObject obj = doc.as<JsonObject>();
  return &obj;
}
