void system_init() {
  Serial.begin(115200);
  Serial.println();
  LogDivisionSta("System Info");
  // file system init
  Serial.println("Little File System 2.0.1");
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    return;
  }
  char ret[100] = { 0 };
  readFile("/hello.txt", ret);
  Serial.println(ret);
  // writeFile("/hello.txt", "Chen Haibin -> @r3inbowari");
  listDir("/");
  LittleFS.end();

  // config init
  Serial.println("Loading device configuration.");
  loadConfiguration("config.json", config);
  Serial.print("Device Type: ");
  Serial.println(config.type);
  Serial.print("Device ID: ");
  Serial.println(config.id);
  Serial.print("Device Version: ");
  Serial.println(String(config.major) + "." + String(config.minor) + "." + String(config.patch));

  Serial.print("SSID: ");
  Serial.println(config.ssid);

  Serial.print("Mesh Prefix: ");
  Serial.println(config.mesh_prefix);
  Serial.print("Mesh Port: ");
  Serial.println(config.mesh_port);
  Serial.print("Mesh Password: ");
  Serial.println(config.mesh_password);

  Serial.print("TCP Host: ");
  Serial.println(config.tcp_host);
  Serial.print("Update Host: ");
  Serial.println(config.update_host);

  Serial.print("WiFi Mode: ");
  Serial.println(config.wifi_mode);

  Serial.print("MQTT Broker: ");
  Serial.println(config.mqtt_broker);
  Serial.print("MQTT Port: ");
  Serial.println(config.mqtt_port);
  Serial.print("MQTT Username: ");
  Serial.println(config.mqtt_username);
  Serial.print("MQTT Password: ");
  Serial.println(config.mqtt_password);
  LogDivisionEnd("System Info");
}

void sta_init() {
  LogDivisionSta("SA Mode");
  // WiFi.mode(WIFI_AP);
  WiFi.begin(config.ssid, config.password);
  int count = 0;
  Serial.print("Connecting to ");
  Serial.print(config.ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    count ++;
    Serial.print(".");
    if (count > 40) {
      Serial.print("Connected failed");
      return;
    }
  }
  Serial.println("");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  LogDivisionEnd("SA Mode");
}

#include <ESP8266WebServer.h>
ESP8266WebServer server(80);

void ap_init() {
  LogDivisionSta("AP Mode");
  btn.attachClick(handleClick);
  //  WiFi.mode(WIFI_AP_STA);
  //  IPAddress softLocal(192, 168, 128, 1);
  //  IPAddress softGateway(192, 168, 128, 1);
  //  IPAddress softSubnet(255, 255, 255, 0);
  //  WiFi.softAPConfig(softLocal, softGateway, softSubnet);
  //  String apName = ("ESP8266_" + (String)ESP.getChipId());
  //  const char *softAPName = apName.c_str();
  //  WiFi.softAP(softAPName, "12345678");
  //  IPAddress myIP = WiFi.softAPIP();
  //  Serial.println(myIP);
  Serial.println("WiFi Mode: AP");
  Serial.println("Configuring access point...");
  String apName = ("ESP8266_" + (String)ESP.getChipId());
  const char *softAPName = apName.c_str();
  WiFi.softAP(softAPName, "12345678");
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
  LogDivisionEnd("AP Mode");
}

void http_update() {
  server.handleClient();
}

void handleRoot() {
  server.send(200, "text/plain", "It is a RESTful Web created by ESP8266");
}
