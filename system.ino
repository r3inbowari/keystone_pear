#include "system_0x16.h"

void system_init() {
  hardware_init();
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

void hardware_init() {
  // 串口初始化
  Serial.begin(115200);
  Serial.println();
  // GPIO INIT
  pinMode(14, OUTPUT);
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  digitalWrite(14, HIGH);

  // mode switch init
  btn.attachClick(handleClick);

  // GPIO Coupler init func
  init_jqc();

  // 传感器初始化
  init_display();
  open_animation();
  clearDraw();
  init_ccs();
  init_bme();
  init_apds();

  bme_gather();
  content_bme280();
  apds_gather();
  content_apds9960();
  ccs_gather();
  // content_ccs(); 移植到bme上
  menu_addItem(PIC_CHARGE);
}

const unsigned char animation[] U8X8_PROGMEM = { 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x40, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0xa5, 0xa5, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x42, 0xa0, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x02, 0x44, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x51, 0x49, 0x55, 0x55, 0x55, 0x55, 0x55, 0x01, 0x89, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x42, 0x51, 0x55, 0x55, 0x55, 0x55, 0x55, 0x40, 0x10, 0xa5, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x5e, 0xad, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x00, 0x45, 0x40, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x51, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x50, 0x28, 0x15, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x56, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x80, 0x92, 0x48, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x15, 0x20, 0x44, 0x92, 0xa8, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x2a, 0x84, 0x92, 0x24, 0xa5, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x1a, 0x20, 0x49, 0x92, 0xa8, 0xaa, 0x5a, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x05, 0x40, 0x24, 0x49, 0x42, 0x55, 0x55, 0xad, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x8a, 0x14, 0x88, 0x24, 0x55, 0x55, 0xab, 0xaa, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x05, 0x00, 0x25, 0x92, 0x48, 0x55, 0x55, 0x55, 0xb5, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x02, 0x49, 0x90, 0x48, 0x52, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x05, 0x84, 0x0a, 0x25, 0x49, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0xad, 0xaa, 0xaa, 0x42, 0x51, 0xa0, 0x90, 0xa4, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x01, 0x00, 0x0a, 0x0a, 0x89, 0xaa, 0xaa, 0xaa, 0xaa, 0x6a, 0x55, 0x55, 0x55, 0x55, 0xb5, 0xaa, 0x40, 0x4a, 0xa4, 0x50, 0x52, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x01, 0x01, 0x11, 0x85, 0xa4, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x40, 0x40, 0x22, 0x52, 0x52, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x10, 0x10, 0x44, 0x89, 0x44, 0x55, 0x55, 0x55, 0x55, 0x55, 0xd5, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x01, 0x82, 0x28, 0x24, 0x29, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x01, 0x00, 0x42, 0x51, 0x92, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x22, 0x55, 0xab, 0xaa, 0xaa, 0xaa, 0x82, 0x20, 0x29, 0x82, 0x24, 0x55, 0x55, 0x55, 0x55, 0x55, 0x05, 0xd5, 0x54, 0x55, 0x55, 0x55, 0x05, 0x04, 0x80, 0x54, 0x4a, 0x55, 0x55, 0x55, 0x55, 0x55, 0x01, 0x55, 0xb5, 0xaa, 0xaa, 0x54, 0x15, 0x41, 0x50, 0x82, 0x10, 0xaa, 0xaa, 0xaa, 0xa4, 0xaa, 0x24, 0x55, 0x55, 0x55, 0x15, 0x50, 0x15, 0x10, 0x02, 0x54, 0x4a, 0x55, 0x55, 0x15, 0xa9, 0x2a, 0x01, 0xaa, 0xaa, 0xaa, 0x0a, 0xa0, 0x84, 0x04, 0xa8, 0x0a, 0x91, 0x54, 0x55, 0x55, 0x52, 0x15, 0x00, 0x55, 0x55, 0x55, 0x05, 0x42, 0x35, 0x00, 0x29, 0x59, 0x24, 0xa9, 0xaa, 0x92, 0x04, 0x0a, 0x00, 0x55, 0x55, 0x55, 0x81, 0x00, 0x0a, 0x40, 0x08, 0x84, 0x88, 0x54, 0x55, 0x09, 0x52, 0x01, 0x00, 0x55, 0xa9, 0x20, 0x00, 0x88, 0x94, 0x08, 0x44, 0x51, 0x22, 0x52, 0x15, 0x52, 0x08, 0x44, 0x24, 0x55, 0x55, 0x00, 0x10, 0x22, 0x00, 0x82, 0x08, 0x14, 0x44, 0x80, 0xa2, 0x04, 0x42, 0x00, 0x00, 0x05, 0x15, 0x08, 0x82, 0x00, 0x24, 0x10, 0x52, 0xa1, 0x08, 0x21, 0x09, 0x00, 0x00, 0x00, 0x02, 0x22, 0x15, 0x40, 0x08, 0xaa, 0x80, 0x44, 0x14, 0x14, 0x21, 0x04, 0x22, 0x89, 0x00, 0x22, 0x00, 0x01, 0x09, 0x11, 0xa1, 0x00, 0x0a, 0x10, 0x91, 0x12, 0x8a, 0x48, 0x08, 0x12, 0x20, 0x00, 0x20, 0x08, 0x00, 0x04, 0x04, 0x24, 0xa1, 0x42, 0x24, 0xa8, 0x20, 0x02, 0xa1, 0x10, 0x09, 0x80, 0x04, 0x40, 0x40, 0x20, 0x91, 0x08, 0x08, 0x14, 0x49, 0x05, 0x82, 0x28, 0x24, 0xb4, 0x01, 0x12, 0x40, 0x01, 0x12, 0x04, 0x00, 0xa2, 0x42, 0x21, 0x12, 0x50, 0x28, 0x42, 0xd1, 0x22, 0x24, 0x00, 0x11, 0x04, 0x00, 0x51, 0x55, 0x08, 0x08, 0x44, 0xa4, 0x2a, 0x81, 0x08, 0xa4, 0xe8, 0x90, 0x24, 0x80, 0x90, 0x08, 0x00, 0x80, 0x42, 0xa2, 0x88, 0x12, 0x08, 0x0a, 0xa2, 0x28, 0x92, 0x06, 0x81, 0x24, 0x00, 0x42, 0x4a, 0x12, 0x88, 0x08, 0x51, 0x40, 0x21, 0x20, 0x00, 0x42, 0xd4, 0x2a, 0x08, 0x80, 0x4a, 0x10, 0x21, 0x44, 0x21, 0x22, 0x02, 0x94, 0x4a, 0x81, 0x94, 0x90, 0x54, 0x45, 0x25, 0x82, 0x20, 0x45, 0x88, 0x10, 0x84, 0x48, 0x00, 0x51, 0x94, 0x14, 0x00, 0x44, 0x2b, 0x1d, 0x80, 0x08, 0x08, 0x10, 0x21, 0xa2, 0x28, 0x02, 0x54, 0x8a, 0x22, 0xa5, 0xaa, 0x90, 0xd4, 0x4a, 0x09, 0x20, 0xa1, 0x42, 0x84, 0x04, 0x02, 0x90, 0x82, 0x20, 0x49, 0x4a, 0x01, 0x44, 0x2a, 0x15, 0xa2, 0x92, 0x14, 0x14, 0x11, 0x20, 0x80, 0x24, 0x28, 0x4a, 0x92, 0x90, 0x94, 0x11, 0xa9, 0x25, 0x21, 0x04, 0x42, 0x41, 0x40, 0x02, 0x29, 0x80, 0x92, 0x54, 0x25, 0x25, 0x22, 0x44, 0x52, 0x03, 0x88, 0xa0, 0x08, 0x00, 0x14, 0x48, 0x84, 0x2a, 0xa5, 0xaa, 0x52, 0x8a, 0x88, 0x90, 0xe8, 0xa8, 0x02, 0x12, 0x00, 0x14, 0x80, 0x02, 0x11, 0x44, 0x4a, 0x02, 0xa4, 0x50, 0x12, 0x05, 0xb1, 0x01, 0xa8, 0x40, 0x52, 0x81, 0x08, 0xa8, 0x4a, 0x91, 0x90, 0xa8, 0x49, 0x8d, 0xa4, 0xe8, 0xda, 0x24, 0x01, 0x0a, 0x04, 0x48, 0xa0, 0x22, 0x00, 0x25, 0x25, 0x55, 0x96, 0x22, 0x49, 0x52, 0x31, 0x10, 0x4a, 0x20, 0x51, 0x01, 0x15, 0x90, 0xaa, 0x88, 0x48, 0xa2, 0x20, 0x14, 0x02, 0xa9, 0x75, 0x45, 0x90, 0x4a, 0x0a, 0x54, 0x40, 0x45, 0x10, 0x35, 0x92, 0x14, 0xab, 0xa2, 0x54, 0xa0, 0x50, 0x90, 0x0a, 0x10, 0xa0, 0x02, 0x25, 0x10, 0x4a, 0x42, 0x25, 0xa5, 0x84, 0x54, 0x09, 0x32, 0xeb, 0x25, 0xa4, 0x4a, 0x05, 0x29, 0x80, 0x4a, 0x92, 0x28, 0xd5, 0xaa, 0x2a, 0x25, 0x52, 0xa4, 0x35, 0x55, 0x49, 0x24, 0x68, 0x40, 0x29, 0x90, 0xa4, 0x4a, 0x55, 0x11, 0x44, 0x4a, 0xa5, 0x28, 0xd5, 0xaa, 0x92, 0x08, 0x93, 0x2a, 0x42, 0x0b, 0xa9, 0x52, 0x92, 0x44, 0xa9, 0x90, 0x48, 0xf2, 0x56, 0x15, 0x24, 0x55, 0x54, 0x92, 0x54, 0x50, 0x05, 0x89, 0x24, 0x2a, 0x05, 0x2b, 0x55, 0xa5, 0x2a, 0xa2, 0x12, 0x00, 0x85, 0x44, 0x24, 0x09, 0xaa, 0x50, 0x89, 0x50, 0xda, 0x54, 0x15, 0x08, 0x41, 0x49, 0xa4, 0x2a, 0x28, 0x89, 0x48, 0x92, 0x20, 0x0a, 0x52, 0x95, 0x12, 0x55, 0x49, 0xa5, 0x94, 0x14, 0x09, 0x84, 0x44, 0x22, 0x29, 0x49, 0x8a, 0x50, 0x09, 0x55, 0x55, 0x42, 0x84, 0x48, 0x2a, 0x40, 0xa2, 0x12, 0x08, 0x94, 0x04, 0x02, 0x14, 0x25, 0x52, 0xaa, 0x92, 0x94, 0x10, 0x81, 0x80, 0x14, 0x89, 0x40 };
void open_animation() {
  u8g2.drawXBMP(0, 0, 128, 64, animation);
  u8g2.sendBuffer();
  delay(random(2000, 4000));

  u8g2.setDrawColor(0);
  u8g2.drawBox(24, 40, 80, 16); // 0-23 24-103 105-127
  u8g2.setDrawColor(1);
  u8g2.drawFrame(25, 41, 78, 14);
  u8g2.sendBuffer();
  delay(1000);
  for (int i = 0; i < 24; i ++) {
    u8g2.drawLine(27 + i, 43, 27 + i, 52);
    u8g2.sendBuffer();
    delay(random(1, 3) * 20);
  }
  for (int i = 24; i < 49; i ++) {
    u8g2.drawLine(27 + i, 43, 27 + i, 52);
    u8g2.sendBuffer();
    delay(random(1, 6) * 20);
  }
  for (int i = 49; i < 74; i ++) {
    u8g2.drawLine(27 + i, 43, 27 + i, 52);
    u8g2.sendBuffer();
    delay(random(1, 9) * 20);
  }
  delay(random(600, 900));
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

void ap_update() {
  btn.tick();
}

void ap_init() {
  LogDivisionSta("AP Mode");
  menu_addItem(PIC_ACCESS_POINT);
  WiFi.mode(WIFI_AP_STA);
  IPAddress softLocal(192, 168, 128, 1);
  IPAddress softGateway(192, 168, 128, 1);
  IPAddress softSubnet(255, 255, 255, 0);
  WiFi.softAPConfig(softLocal, softGateway, softSubnet);
  String apName = ("ESP8266_" + (String)ESP.getChipId());
  const char *softAPName = apName.c_str();
  WiFi.softAP(softAPName, "12345678");
  IPAddress myIP = WiFi.softAPIP();
  Serial.println("WiFi Mode: AP");
  Serial.println("Configuring access point...");
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.on("/changeWlan", handleWlan);
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

/**
   WLAN修改
   @rest ip:port/changeWlan?ssid=[ssid]&password=[password]
*/
void handleWlan() {
  if (server.hasArg("ssid") && server.hasArg("password")) {
    String arg0 = server.arg("ssid");
    String arg1 = server.arg("password");
    server.send(200, "text/plain", "wlan params: " + arg0 + ", " + arg1);
    config.ssid = arg0;
    config.password = arg1;
    Serial.println("WLAN configuration has been change.");
    Serial.println("Saving configuration now.");
    saveConfiguration("config.json", config);
    Serial.println("ESP8266 will restart to apply the new setting.");
    ESP.restart();
  } else {
    server.send(200, "text/plain", "error or less query params.");
  }
}
