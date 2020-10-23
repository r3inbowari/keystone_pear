#include "ccs.h"

int co2 = 0;
int tvoc = 0;

CCS811 mySensor(CCS811_ADDR);

void init_ccs() {
  Serial.println(F("--------------------------------"));
  Serial.println(F("SparkFun CCS-811 - CO2Sensor"));
  Serial.println(F("--------------------------------"));
  Wire.begin();
  if (mySensor.begin() == false)
  {
    Serial.print("CCS811 error. Please check wiring. Freezing...");
    while (1)
      ;
  }
  Serial.println(F("CSS811 sensor is now running"));
  Serial.println(F("--------------------------------"));
}

void ccs_gather() {
  if (mySensor.dataAvailable())
  {
    //If so, have the sensor read and calculate the results.
    //Get them later
    mySensor.readAlgorithmResults();
    co2 = mySensor.getCO2();
    tvoc = mySensor.getTVOC();
    Serial.print("[CCS] ");
    Serial.print("CO2: ");
    Serial.print(String(co2));
    Serial.print("ppm, TVOC: ");
    Serial.println(String(tvoc));
  }
}
