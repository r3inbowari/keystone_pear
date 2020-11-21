#include <LittleFS.h>

// I think this is because painlessMesh uses ArduinoJson too and this causes problems in the import order. --chenhaibin 20200912
// @see https://gitlab.com/painlessMesh/painlessMesh/-/issues/363
#include "painlessMesh.h"
#include "painlessmesh/layout.hpp"
#include <ArduinoJson.h>

#include <ESP8266httpUpdate.h>
#include "conf.h"
#include "mesh.h"
#include "mq.h"
#include "ESPAsyncTCP.h"
#include "SyncClient.h"
#include "enum.h"
#include "display.h"
#include "system_0x16.h"

void setup() {
  system_init();

  // https://gitlab.com/painlessMesh/painlessMesh/-/wikis/api#painlessmeshstationmanual-string-ssid-string-password-uint16_t-port-uint8_t-remote_ip-
  // sta_init();


  // String conf = "{\"id\":\"fa38c7e7-865d-4322-b693-d9f715857d2a\",\"type\":\"pear\",\"major\":1,\"minor\":5,\"patch\":8,\"ssid\":\"CU_Config\",\"password\":\"15946395951\",\"update_host\":\"http://r3inbowari.top:1998\",\"tcp_host\":\"192.168.0.102:1030\",\"mesh_prefix\":\"pear\",\"mesh_password\":\"r3inbowari\",\"mesh_port\":5555,\"wifi_mode\":0,\"mqtt_broker\":\"r3inbowari.top\",\"mqtt_port\":1883,\"mqtt_username\":\"r3inb\",\"mqtt_password\":\"159463\"}";
  //     setConfig(conf.c_str());

  // String conf = "{\"id\":\"fa38c7e7-865d-4322-b693-d9f715857d2a\",\"type\":\"pear\",\"major\":1,\"minor\":5,\"patch\":8,\"ssid\":\"dongyajing\",\"password\":\"999999999\",\"update_host\":\"http://r3inbowari.top:3000\",\"tcp_host\":\"192.168.0.102:1030\",\"mesh_prefix\":\"pear\",\"mesh_password\":\"r3inbowari\",\"mesh_port\":5555,\"wifi_mode\":0,\"mqtt_broker\":\"r3inbowari.top\",\"mqtt_port\":1883,\"mqtt_username\":\"r3inb\",\"mqtt_password\":\"159463\"}";
  // setConfig(conf.c_str());

  // String conf = "{\"id\":\"fa38c7e7-865d-4322-b693-d9f715857d2a\",\"type\":\"pear\",\"major\":1,\"minor\":5,\"patch\":8,\"ssid\":\"ZTE\",\"password\":\"panglixiao\",\"update_host\":\"http://r3inbowari.top:3000\",\"tcp_host\":\"192.168.0.102:1030\",\"mesh_prefix\":\"pear\",\"mesh_password\":\"r3inbowari\",\"mesh_port\":5555,\"wifi_mode\":0,\"mqtt_broker\":\"r3inbowari.top\",\"mqtt_port\":1883,\"mqtt_username\":\"r3inb\",\"mqtt_password\":\"159463\"}";
  // setConfig(conf.c_str());

  // String conf = "{\"id\":\"fa38c7e7-865d-4322-b693-d9f715857d2a\",\"type\":\"pear\",\"major\":1,\"minor\":5,\"patch\":0,\"ssid\":\"\",\"password\":\"\",\"update_host\":\"http://r3inbowari.top:3000\",\"tcp_host\":\"192.168.0.102:1030\",\"mesh_prefix\":\"pear\",\"mesh_password\":\"r3inbowari\",\"mesh_port\":5555,\"wifi_mode\":0,\"mqtt_broker\":\"r3inbowari.top\",\"mqtt_port\":1883,\"mqtt_username\":\"r3inb\",\"mqtt_password\":\"159463\"}";
  // setConfig(conf.c_str());

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
  oled_update();
}




































void test() {
  //  pinMode(16,OUTPUT);
  //  digitalWrite(16, HIGH);

  //  ipv4算法
  //  Serial.println(checkIpv4("ip unset"));
  //  Serial.println(checkIpv4("192.168.1.1"));
  //  Serial.println(checkIpv4("123213213"));
  //  Serial.println(checkIpv4("1236.231"));
  //  Serial.println(checkIpv4("211.23.23.1"));
  //  Serial.println(checkIpv4("211.2113.23.1"));
  //  Serial.println(checkIpv4("211.255.23.1"));
  //  Serial.println(checkIpv4("211.256.23.1"));
  //  Serial.println(checkIpv4("211.221.2123.1"));
  //  Serial.println(checkIpv4("211.23.23.0"));
  //  Serial.println(checkIpv4("211.23.134.-1"));
  //  Serial.println(checkIpv4("!@##%dfda"));

  //  版本差异性算法测试
  //  Serial.println(version_diff(1, 2, 3, 1, 2, 4));
  //  Serial.println(version_diff(2, 3, 3, 3, 2, 2));
  //  Serial.println(version_diff(3, 2, 3, 2, 3, 8));
  //  Serial.println(version_diff(6, 5, 3, 4, 2, 5));
  //  Serial.println(version_diff(7, 2, 3, 8, 4, 3));
  //  Serial.println(version_diff(7, 2, 3, 7, 2, 3));

  //  json写入测试
  //        String conf = {\"id\":\"fa38c7e7-865d-4322-b693-d9f715857d2a\",\"type\":\"pear\",\"major\":1,\"minor\":5,\"patch\":0,\"ssid\":\"Xiaomi_D909\",\"password\":\"12345678\",\"update_host\":\"http://192.168.0.102:3000\",\"tcp_host\":\"192.168.0.102:1030\",\"mesh_prefix\":\"pear\",\"mesh_password\":\"r3inbowari\",\"mesh_port\":5555,\"wifi_mode\":0,\"mqtt_broker\":\"r3inbowari.top\",\"mqtt_port\":1883,\"mqtt_username\":\"r3inb\",\"mqtt_password\":\"159463\"}"";
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
}
