/**
   config behavior header
   @author r3inbowari
   @date 2020/6/21
*/

#ifndef __CONF_H
#define __CONF_H
extern struct Config {
  String type;
  String id;
  int major;
  int minor;
  int patch;
  String ssid;
  String password;
  String update_host;
  String tcp_host;
  
  String mesh_prefix;
  String mesh_password;
  int mesh_port;
  
  int wifi_mode;
  
  String mqtt_broker;
  int mqtt_port;
  String mqtt_username;
  String mqtt_password;
} config;

extern const char *filename ;  // <- SD library uses 8.3 filenames

extern void loadConfiguration(const char *filename, Config &config);

#endif
