#include <ESP8266WiFi.h>
#include <ESP8266WiFiMesh.h>
#include <TypeConversionFunctions.h>
#include <assert.h>

#include <OneButton.h>
#include "node.h"

#define BUTTON_PIN 16 // button maping

// Handler function for a single click:
#define MESH_LED 14

OneButton btn = OneButton(
                  BUTTON_PIN,  // Input pin for the button
                  false,       // Button is active high
                  false        // Disable internal pull-up resistor
                );

// mesh
const char exampleMeshName[] PROGMEM = "MeshNode_";
const char exampleWiFiPassword[] PROGMEM = "ChangeThisWiFiPassword_TODO";

unsigned int requestNumber = 0;
unsigned int responseNumber = 0;

String manageRequest(const String &request, ESP8266WiFiMesh &meshInstance);
transmission_status_t manageResponse(const String &response, ESP8266WiFiMesh &meshInstance);
void networkFilter(int numberOfNetworks, ESP8266WiFiMesh &meshInstance);

/* Create the mesh node object */
ESP8266WiFiMesh meshNode = ESP8266WiFiMesh(manageRequest, manageResponse, networkFilter, FPSTR(exampleWiFiPassword), FPSTR(exampleMeshName), "", true);

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

//static void handleClick() {
//  Serial.println("Clicked Mesh Button");
//
//  if (mesh_flag) {
//    // close mesh
//    Serial.println("Status: Open -> Close");
//
//    // close mesh led
//    digitalWrite(MESH_LED, HIGH);
//    mesh_flag = false;
//
//    // close this node
//    meshNode.deactivateAP();
//
//  } else {
//    // open mesh
//    Serial.println("Status: Close -> Open");
//
//    // open mesh led
//    digitalWrite(MESH_LED, LOW);
//    mesh_flag = true;
//
//    // open this node
//    meshNode.activateAP(); // Each AP requires a separate server port.
//  }
//}

/**
   Callback for when other nodes send you a request

   @param request The request string received from another node in the mesh
   @param meshInstance The ESP8266WiFiMesh instance that called the function.
   @returns The string to send back to the other node
*/
String manageRequest(const String &request, ESP8266WiFiMesh &meshInstance) {
  // We do not store strings in flash (via F()) in this function.
  // The reason is that the other node will be waiting for our response,
  // so keeping the strings in RAM will give a (small) improvement in response time.
  // Of course, it is advised to adjust this approach based on RAM requirements.


  Serial.println("------------------------------------");
  
  /* Print out received message */
  Serial.print("Request received: ");
  Serial.println(request);

  /* return a string to send back */
  return ("Hello world response #" + String(responseNumber++) + " from " + meshInstance.getMeshName() + meshInstance.getNodeID() + ".");
}

/**
   Callback for when you get a response from other nodes

   @param response The response string received from another node in the mesh
   @param meshInstance The ESP8266WiFiMesh instance that called the function.
   @returns The status code resulting from the response, as an int
*/
transmission_status_t manageResponse(const String &response, ESP8266WiFiMesh &meshInstance) {

  Serial.println("------------------------aaaaaaaaaa------------");
  
  transmission_status_t statusCode = TS_TRANSMISSION_COMPLETE;

  /* Print out received message */
  Serial.print(F("Request sent: "));
  Serial.println(meshInstance.getMessage());
  Serial.print(F("Response received: "));
  Serial.println(response);

  // Our last request got a response, so time to create a new request.
  meshInstance.setMessage(String(F("Hello world request #")) + String(++requestNumber) + String(F(" from "))
                          + meshInstance.getMeshName() + meshInstance.getNodeID() + String(F(".")));

  // (void)meshInstance; // This is useful to remove a "unused parameter" compiler warning. Does nothing else.
  return statusCode;
}

/**
   Callback used to decide which networks to connect to once a WiFi scan has been completed.

   @param numberOfNetworks The number of networks found in the WiFi scan.
   @param meshInstance The ESP8266WiFiMesh instance that called the function.
*/
void networkFilter(int numberOfNetworks, ESP8266WiFiMesh &meshInstance) {
  Serial.println(String(numberOfNetworks));

  for (int networkIndex = 0; networkIndex < numberOfNetworks; ++networkIndex) {
    String currentSSID = WiFi.SSID(networkIndex);
    int meshNameIndex = currentSSID.indexOf(meshInstance.getMeshName());

    /* Connect to any _suitable_ APs which contain meshInstance.getMeshName() */
    if (meshNameIndex >= 0) {
      uint64_t targetNodeID = stringToUint64(currentSSID.substring(meshNameIndex + meshInstance.getMeshName().length()));

      if (targetNodeID < stringToUint64(meshInstance.getNodeID())) {
        ESP8266WiFiMesh::connectionQueue.push_back(NetworkInfo(networkIndex));
      }
    }
  }
}

/**
   mesh init func
*/
void node_init() {
  LogDivisionSta("Mesh Mode");
  Serial.println("WiFi Mode: Mesh");
  btn.attachClick(handleClick);

  WiFi.persistent(false);
  WiFi.disconnect();
  Serial.println(F("Mesh Network Node 2.1 Copyright (c) 2015 Julian Fell. All rights reserved."));
  meshNode.begin();
  //meshNode.setStaticIP(IPAddress(192, 168, 4, 26)); // Activate static IP mode to speed up connection times.
  meshNode.activateAP();
  LogDivisionEnd("Mesh Mode");
}

int32_t timeOfLastScan = -10000;

/**
   loop routine
*/
void node_update() {
  btn.tick();
  // if (mesh_flag) {
  // 模式 1 -> 开启 mesh 处理程序
  if (config.wifi_mode == 1) {
    if (millis() - timeOfLastScan > 3000 // Give other nodes some time to connect between data transfers.
        || (WiFi.status() != WL_CONNECTED && millis() - timeOfLastScan > 2000)) { // Scan for networks with two second intervals when not already connected.
      // String request = String(F("Hello world request #")) + String(requestNumber) + String(F(" from ")) + meshNode.getMeshName() + meshNode.getNodeID() + String(F("."));
      String request = String(F("HEART"));
      meshNode.attemptTransmission(request, false);
      timeOfLastScan = millis();

      // One way to check how attemptTransmission worked out
//      if (ESP8266WiFiMesh::latestTransmissionSuccessful()) {
//        Serial.println(F("Transmission successful."));
//      }
//
//      // Another way to check how attemptTransmission worked out
//      if (ESP8266WiFiMesh::latestTransmissionOutcomes.empty()) {
//        Serial.println(F("No mesh AP found."));
//      } else {
//        for (TransmissionResult &transmissionResult : ESP8266WiFiMesh::latestTransmissionOutcomes) {
//          if (transmissionResult.transmissionStatus == TS_TRANSMISSION_FAILED) {
//            Serial.println(String(F("Transmission failed to mesh AP ")) + transmissionResult.SSID);
//          } else if (transmissionResult.transmissionStatus == TS_CONNECTION_FAILED) {
//            Serial.println(String(F("Connection failed to mesh AP ")) + transmissionResult.SSID);
//          } else if (transmissionResult.transmissionStatus == TS_TRANSMISSION_COMPLETE) {
//            // No need to do anything, transmission was successful.
//            Serial.println("112323")
//          } else {
//            Serial.println(String(F("Invalid transmission status for ")) + transmissionResult.SSID + String(F("!")));
//            assert(F("Invalid transmission status returned from responseHandler!") && false);
//          }
//        }
//      }
      Serial.println();
    } else {
      /* Accept any incoming connections */
      meshNode.acceptRequest();
    }
  }
}
