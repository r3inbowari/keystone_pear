//#define OTA_PART_SIZE 1024
//#define HARDWARE_TYPE "ESP8266"
//
//void offerStart(String role, File entry) {
//  mesh.initOTASend(
//  [&entry](painlessmesh::plugin::ota::DataRequest pkg, char* buffer) {
//    entry.seek(OTA_PART_SIZE * pkg.partNo);
//    entry.readBytes(buffer, OTA_PART_SIZE);
//    return min((unsigned)OTA_PART_SIZE, entry.size() - (OTA_PART_SIZE * pkg.partNo));
//  }, OTA_PART_SIZE);
//  MD5Builder md5;
//  md5.begin();
//  md5.addStream(entry, entry.size());
//  md5.calculate();
//  mesh.offerOTA(role, HARDWARE_TYPE, md5.toString(), ceil(((float)entry.size()) / OTA_PART_SIZE), false);
//}
