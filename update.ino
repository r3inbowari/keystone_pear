/**
   server update realize
   @author r3inbowari
   @date 2020/6/24
*/
#include "conf.h"

#define OS "esp8266"

int remote_major = 1;
int remote_minor = 5;
int remote_patch = 2;

void check_update() {
  LogDivisionSta("Update Checking");
  Serial.print("Local version: ");
  Serial.println(String(config.major) + "." + String(config.minor) + "." + String(config.patch));
  Serial.print("SHA1: ");
  Serial.println(sha1(String(config.major) + "." + String(config.minor) + "." + String(config.patch)));
  Serial.print("Remote version: ");
  Serial.println(String(remote_major) + "." + String(remote_minor) + "." + String(remote_patch));
  Serial.print("SHA1: ");
  Serial.println(sha1(String(remote_major) + "." + String(remote_minor) + "." + String(remote_patch)));
  
  int check_tag = version_diff(config.major, config.minor, config.patch, remote_major, remote_minor, remote_patch);
  if (check_tag == -1) {
    Serial.println("Found new update.");
    String updatePath = config.update_host + "/" + OS + "/" + config.type + "_v5_" + String(remote_major) + "." + String(remote_minor) + "." + String(remote_patch) + ".bin";
    __esp_update(updatePath);
  } else if (check_tag == 1) {
    Serial.println("The local version is ahead of the remote branch.");
    Serial.println("Maybe this is a beta version.");
  } else {
    Serial.println("No updates available.");
  }
  LogDivisionEnd("Update Checking");
}

/**
   @return 1 超前版本 -1 落后版本 0 无差异版本
*/
int version_diff(int remote_major, int remote_minor, int remote_patch, int local_major, int local_minor, int local_patch )
{
  int client_version[3] = { remote_major, remote_minor, remote_patch};
  int base_version[3] = { local_major, local_minor, local_patch };

  int diff = 0;
  diff += (client_version[0] - base_version[0]) << 20;
  diff += (client_version[1] - base_version[1]) << 10;
  diff += (client_version[2] - base_version[2]);
  if (diff > 0) {
    return 1;
  }
  else if (diff == 0) {
    return 0;
  }
  else {
    return -1;
  }
}

void __esp_update(String updatePath) {
  WiFiClient client;
  ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);
  ESPhttpUpdate.onStart(update_started);
  ESPhttpUpdate.onEnd(update_finished);
  ESPhttpUpdate.onProgress(update_progress);
  ESPhttpUpdate.onError(update_error);

  Serial.println("Redirect to " + updatePath);
  t_httpUpdate_return ret = ESPhttpUpdate.update(client, updatePath);

  switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      break;

    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("HTTP_UPDATE_NO_UPDATES");
      break;

    case HTTP_UPDATE_OK:
      Serial.println("HTTP_UPDATE_OK");
      break;
  }
}

void update_started() {
  Serial.println("CALLBACK:  HTTP update process started");
}

void update_finished() {
  Serial.println("CALLBACK:  HTTP update process finished");
}

void update_progress(int cur, int total) {
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}

void update_error(int err) {
  Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);

  config.major = remote_major;
  config.minor = remote_minor;
  config.patch = remote_patch;
  
  saveConfiguration("config.json", config);

  Serial.println(String(remote_major));
}
