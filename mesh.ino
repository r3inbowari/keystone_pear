#include "mq.h"
#include "mesh.h"
#include "utils.h"
#include "display.h"

#define BUTTON_PIN 16 // button maping
#define MESH_LED 14  //single click

//#define   MESH_PREFIX     "whateverYouLike"
//#define   MESH_PASSWORD   "somethingSneaky"
//#define   MESH_PORT       5555

Scheduler userScheduler;
painlessMesh  mesh;

OneButton btn = OneButton(
                  BUTTON_PIN,  // Input pin for the button
                  false,       // Button is active high
                  false        // Disable internal pull-up resistor
                );
// ntp udp
static WiFiUDP udp;

// User stub
void sendMessage();


bool rootNode = false;
size_t rootId = 0;

// WIFI网络状态图标锁定
bool network_flag = false;

// apds9960 定时任务
Task taskAPDS( TASK_SECOND * 4 , TASK_FOREVER, &runAPDSGather );
// apds轮播flag切换任务
Task taskAPDSSwiper( TASK_SECOND * 9 , TASK_FOREVER, &runAPDSSwiper );

// bme280 定时任务
Task taskBME( TASK_SECOND * 3 , TASK_FOREVER, &runBMEGather );
// bme280轮播flag切换任务
Task taskBMESwiper( TASK_SECOND * 5 , TASK_FOREVER, &runBMESwiper );

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );
Task taskTimeSync( TASK_HOUR , TASK_FOREVER, &timeSync );
Task taskTimeBroadcast( TASK_SECOND * 30 , TASK_FOREVER, &timeBroadcast );
Task taskIRTest( TASK_SECOND * 5 , TASK_FOREVER, &runIRTest );
Task taskTimeDisplay( TASK_SECOND * 10 , TASK_FOREVER, &runTimeDisplay );

Task taskWLANSearchDisplay( TASK_SECOND * 1 , TASK_FOREVER, &runWLANDisplay );

Task taskLogon( TASK_MINUTE * 1 , TASK_FOREVER, &runLogon );

void runLogon() {
  onUpload();
  String payload = "{\"id\":\"" + config.id + "\",\"code\":0,\"operation\":" + OP::LogonRequest + ",\"data\":\"logon request\"}";
  mqttClient.publish("meshNetwork/from/rootNode/logon", payload.c_str());
  onUpload();
}

void runWLANDisplay() {
  static int wlan_step = 0;
  if (wlan_step > 5) {
    wlan_step = 0;
  }
  if (!network_flag) {
    if (wlan_step == 0) {
      menu_delItem(PIC_WIFI_SEARCH_2);
      menu_addItem(PIC_WIFI_SEARCH_1);
    } else if (wlan_step == 1) {
      menu_delItem(PIC_WIFI_SEARCH_1);
      menu_addItem(PIC_WIFI_SEARCH_2);
    } else if (wlan_step == 2) {
      menu_delItem(PIC_WIFI_SEARCH_2);
      menu_addItem(PIC_WIFI_SEARCH_3);
    } else if (wlan_step == 3) {
      menu_delItem(PIC_WIFI_SEARCH_3);
      menu_addItem(PIC_WIFI);
    } else if (wlan_step == 4) {
      menu_delItem(PIC_WIFI);
      menu_addItem(PIC_WIFI_SEARCH_3);
    } else if (wlan_step == 5) {
      menu_delItem(PIC_WIFI_SEARCH_3);
      menu_addItem(PIC_WIFI_SEARCH_2);
    }
  }
  wlan_step ++;
}

void onUpload() {
  static bool up_flag = false;
  if (up_flag) {
    menu_delItem(PIC_UPLOAD);
  } else {
    menu_addItem(PIC_UPLOAD);
  }
  up_flag = !up_flag;
}

void runTimeDisplay() {
  Serial.println("[CRON] Time display");
  time_t t = now();
  int hh = (hour(t) + 8) % 24;
  int mm = minute(t);
  menu_timePrint(hh, mm);
}

void runBMEGather() {
  Serial.printf("[CRON]");
  bme_gather();
  content_bme280();
  // mqtt 发布
  String dat = String(tem) + "," + String(hum) + "," + String(pre) + "," + String(alt);
  auto ts = now();
  INFOL("Send APDS From " + String(mesh.getNodeId()) + " to " + String(rootId));
  String msg = "{\"id\":\"" + config.id + "\",\"mid\":\"" + mesh.getNodeId() + "\",\"ts\":" + ts + ",\"operation\":" + OP::SensorData + ",\"data\":{\"type\":2,\"measure\":[" + dat + "]}}";
  Serial.println(msg);
  if (ts > 1599900000) {
    if (mesh.getNodeId() != rootId && rootId != 0) {
      INFOH(" Done.");
      mesh.sendSingle(rootId, msg);
    } else {
      // send to mqtt
      INFOH(" Direct routing.");
      onUpload();
      mqttClient.publish("meshNetwork/from/rootNode/bme", msg.c_str());
      onUpload();
    }
  } else {
    INFOH(" Failed. Due to the unreachable network or The master device is not ready yet.");
  }
}

void runBMESwiper() {
  Serial.println("[CRON][BME] Swiper page index");
  if (bme_swiper_index == BME_TAG::HP)
    bme_swiper_index = BME_TAG::TA;
  else
    bme_swiper_index ++;
  // 切换完毕后, 实时更新到OLED
  content_bme280();
}

void runAPDSGather() {
  Serial.printf("[CRON]");
  apds_gather();
  // 收集完毕后, 实时更新到OLED
  content_apds9960();
  // mqtt 发布
  String dat = String(ambient_light) + "," + String(red_light) + "," + String(green_light) + "," + String(blue_light);
  auto ts = now();
  INFOL("Send APDS From " + String(mesh.getNodeId()) + " to " + String(rootId));
  String msg = "{\"id\":\"" + config.id + "\",\"mid\":\"" + mesh.getNodeId() + "\",\"ts\":" + ts + ",\"operation\":" + OP::SensorData + ",\"data\":{\"type\":1,\"measure\":[" + dat + "]}}";
  Serial.println(msg);
  if (ts > 1599900000) {
    if (mesh.getNodeId() != rootId && rootId != 0) {
      INFOH(" Done.");
      mesh.sendSingle(rootId, msg);
    } else {
      // send to mqtt
      INFOH(" Direct routing.");
      onUpload();
      mqttClient.publish("meshNetwork/from/rootNode/apds", msg.c_str());
      onUpload();
    }
  } else {
    INFOH(" Failed. Due to the unreachable network or The master device is not ready yet.");
  }
}

void runAPDSSwiper() {
  Serial.println("[CRON][APDS] Swiper page index");
  if (apds_swiper_index == APDS_TAG::Blue)
    apds_swiper_index = APDS_TAG::Clear;
  else
    apds_swiper_index ++;
  // 切换完毕后, 实时更新到OLED
  content_apds9960();
}

void runIRTest() {
  auto ts = now();
  INFOL("Send IRTest From " + String(mesh.getNodeId()) + " to " + String(rootId));
  String msg = "{\"id\":\"" + config.id + "\",\"mid\":\"" + mesh.getNodeId() + "\",\"ts\":" + ts + ",\"operation\":" + OP::SensorData + ",\"data\":{\"type\":0,\"measure\":[27.12,21.22]}}";
  if (ts > 1599900000) {
    if (mesh.getNodeId() != rootId && rootId != 0) {
      INFOH(" Done.");
      mesh.sendSingle(rootId, msg);
    } else {
      // send to mqtt
      INFOH(" Direct routing.");
      mqttClient.publish("meshNetwork/from/rootNode/sensor", msg.c_str());
    }
  } else {
    INFOH(" Failed. Due to the unreachable network or The master device is not ready yet.");
  }
}

void sendMessage() {
  String msg = "Hello from node ";
  msg += mesh.getNodeId();
  mesh.sendBroadcast( msg );
  taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
}

/**
   时间同步回调
*/
void timeSync() {
  auto unixTime = ntpUnixTime(udp);
  if (unixTime > 0) {
    if (!network_flag) {
      taskWLANSearchDisplay.disable();
      menu_delItem(PIC_WIFI_SEARCH_1);
      menu_delItem(PIC_WIFI_SEARCH_2);
      menu_delItem(PIC_WIFI_SEARCH_3);
      menu_delItem(PIC_WIFI);
      menu_addItem(PIC_WIFI);
      menu_addItem(PIC_LTE);
      network_flag = true;
    }
    INFOL("Sync timebase to ");
    INFOH(String(unixTime));
    setTime(unixTime);
    INFOH("Enable time display option");
    taskTimeDisplay.enable();
  }
}

/**
   时间广播
*/
void timeBroadcast() {
  auto ts = now();
  if (ts > 1599900000) {
    INFOL("Boardcast TimeBase: ");
    INFOH(String(ts));

    INFOL("Boardcast RootNode: ");
    INFOH(String(mesh.getNodeId()));
    auto payload = "{\"id\":\"" + config.id + "\",\"mid\":\"" + mesh.getNodeId() + "\",\"ts\":" + ts + ",\"operation\":" + OP::TimeBaseBoardcast + ",\"data\":\"\"}";
    mesh.sendBroadcast(payload, false);
  }
}

void receivedCallback( uint32_t from, String &msg ) {
  INFOL("");
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());

  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, msg);
  if (error) {
    WARNL(F("deserializeJson() failed: "));
    WARNH(error.c_str());
    return;
  }
  int op = doc["operation"];
  INFO("Operation Request: " + String(op));

  switch (op) {
    case OP::TimeBaseBoardcast: // 时基/根节点处理
      {
        int ts = doc["ts"];
        setTime(ts);
        INFO("TimeBase has been updated.");

        rootId = doc["mid"];
        INFO("RootNode has been updated.");
      }
      break;

    case OP::SensorData:
      {
        String data = doc["data"];
        INFO(data);
        mqttClient.publish("meshNetwork/from/rootNode/sensor", msg.c_str());
      }
      break;
    default:
      INFO("Invaild request or response.");
  }
}

void newConnectionCallback(uint32_t nodeId) {
  INFO("New Connection, NodeId: " + String(nodeId));
  auto ts = now();
  if (ts > 1599900000) {
    INFO("Send Single TimeBase Reg: " + String(ts));
    // NOTE: Param mid must contain the rootId,
    // bacause a device known the ts,
    // it also known where the rootNode was.
    auto payload = "{\"id\":\"" + config.id + "\",\"mid\":\"" + rootId + "\",\"ts\":" + ts + ",\"operation\":" + OP::TimeBaseBoardcast + ",\"data\":\"\"}";
    mesh.sendSingle(nodeId, payload);
  }
}

void changedConnectionCallback() {
  INFO("NodeTree update");

  if (rootNode && now() > 1599900000) {
    String payload = "{\"id\":\"" + config.id + "\",\"mid\":\"" + mesh.getNodeId() + "\",\"ts\":" + now() + ",\"operation\":" + OP::NetworkTopology + ",\"data\":" + mesh.subConnectionJson() + "}";
    Serial.println(payload);
    mqttClient.publish("meshNetwork/from/rootNode/topology", payload.c_str());
  }
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

/**
   button to switch the ap and mesh ap function
*/
static void handleClick() {
  if (config.wifi_mode == 0) {
    Serial.println("AP: Open -> Close");
    config.wifi_mode = 1;
  } else {
    Serial.println("AP: Close -> Open");
    config.wifi_mode = 0;
  }
  Serial.println("Saving configuration now.");
  saveConfiguration("config.json", config);
  Serial.println("ESP8266 will restart to switch wifi mode.");
  ESP.restart();
}

/**
   mesh init func
*/
void node_init() {
  LogDivisionSta("Mesh Mode");
  Serial.println("GPIO14 -> LED2 ON");
  digitalWrite(14, LOW);
  // mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( config.mesh_prefix, config.mesh_password, &userScheduler, config.mesh_port );

  if (config.ssid != "" && config.password != "") {
    Serial.println("Found STA configuration and switch to master device.");
    mesh.stationManual(config.ssid, config.password);
    mesh.setHostname("MeshNetwork");
  } else {
    Serial.println("Found AP configuration and switch to slave device.");
  }

  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.setContainsRoot();

  Serial.print("NodeID: ");
  Serial.println(mesh.getNodeId());

  userScheduler.addTask( taskTimeSync );
  userScheduler.addTask( taskSendMessage );
  userScheduler.addTask( taskTimeBroadcast );
  userScheduler.addTask( taskIRTest );
  userScheduler.addTask( taskBME );
  userScheduler.addTask( taskBMESwiper );
  userScheduler.addTask( taskAPDS );
  userScheduler.addTask( taskAPDSSwiper );
  userScheduler.addTask( taskTimeDisplay );
  userScheduler.addTask( taskWLANSearchDisplay );
  userScheduler.addTask( taskLogon );

  taskLogon.enable();
  taskWLANSearchDisplay.enable();
  taskBME.enable();
  taskBMESwiper.enable();
  taskSendMessage.enable();
  taskIRTest.enable();
  taskAPDS.enable();
  taskAPDSSwiper.enable();

  // rootNode Redefine: when the first apNode link to router
  if (config.ssid != "" && config.password != "") {
    // this node will become the top of all nodes
    rootNode = true;
    rootId = mesh.getNodeId();
    mesh.setRoot(true);
    Serial.print("This node has became the rootNode: ");
    Serial.println(getRootId(mesh.asNodeTree()));
    taskTimeSync.enable();
    taskTimeBroadcast.enable();

    mqtt_init();
  }
  LogDivisionEnd("Mesh Mode");
}

/**
   loop routine
*/
void node_update() {
  btn.tick();
  if (config.wifi_mode == 1) {
    // timebase polling
    if (now() < 1599900000 && rootNode) {
      timeSync();
    }

    if (rootNode) {
      mqtt_update();
    }
    mesh.update();
  }
}

void time_loop() {
  unsigned long unixTime = ntpUnixTime(udp);
  if (unixTime > 0) {
    Serial.print("Get time: ");
    Serial.println(unixTime);
    setTime(unixTime);
  }
}

// rootNode diff
size_t getRootId(painlessmesh::protocol::NodeTree nodeTree) {
  if (nodeTree.root) return nodeTree.nodeId;
  for (auto && s : nodeTree.subs) {
    auto id = getRootId(s);
    if (id != 0) return id;
  }
  return 0;
}
