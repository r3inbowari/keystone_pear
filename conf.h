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
  String prefix;
  String node_password;
  String mesh_ip;
  int wifi_mode;
} config;

extern const char *filename ;  // <- SD library uses 8.3 filenames

extern void loadConfiguration(const char *filename, Config &config);

#endif
