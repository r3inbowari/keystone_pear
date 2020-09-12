#include <LittleFS.h>
#include <ArduinoJson.h>
#include <ESP8266httpUpdate.h>
#include "conf.h"

void setup() {
  system_init();
  sta_init();
  check_update(); // check update
  
  //  pinMode(16,OUTPUT);
  //  digitalWrite(16, HIGH);

  //  版本差异性算法测试
  //  Serial.println(version_diff(1, 2, 3, 1, 2, 4));
  //  Serial.println(version_diff(2, 3, 3, 3, 2, 2));
  //  Serial.println(version_diff(3, 2, 3, 2, 3, 8));
  //  Serial.println(version_diff(6, 5, 3, 4, 2, 5));
  //  Serial.println(version_diff(7, 2, 3, 8, 4, 3));
  //  Serial.println(version_diff(7, 2, 3, 7, 2, 3));

  //  json写入测试
//        String conf = "{\"id\":\"fa38c7e7-865d-4322-b693-d9f715857d2a\",\"type\":\"pear\",\"major\":1,\"minor\":5,\"patch\":0,\"ssid\":\"Xiaomi_D909\",\"password\":\"12345678\",\"update_host\":\"http://192.168.0.102:3000\",\"tcp_host\":\"192.168.0.102:1030\",\"prefix\":\"pear\",\"node_password\":\"r3inbowari\",\"mesh_ip\":\"192.168.4.22\",\"wifi_mode\":0}";
//        setConfig(conf.c_str());
  //  loadConfiguration("config.json", config);
  //  Serial.println(config.id);
  //  Serial.println(config.type);
  //  Serial.println(config.major);
  //  Serial.println(config.minor);
  //  Serial.println(config.patch);
  //  Serial.println(config.ssid);
  //  Serial.println(config.password);
  //  Serial.println(config.update_host);
  //  Serial.println(config.tcp_host);
  //
  //  config修改测试
  //  config.patch = 28;
  //  saveConfiguration("config.json", config);
  //  loadConfiguration("config.json", config);
  //  Serial.println(config.id);
  //  Serial.println(config.type);
  //  Serial.println(config.major);
  //  Serial.println(config.minor);
  //  Serial.println(config.patch);
  //  Serial.println(config.ssid);
  //  Serial.println(config.password);
  //  Serial.println(config.update_host);
  //  Serial.println(config.tcp_host);

  if (config.wifi_mode == 0) {
    // 0时开启AP
    ap_init();
  } else if (config.wifi_mode == 1) {
    // 1时开启Mesh模式
    node_init();
  }
}

void loop() {
  http_update();
  node_update();
}
