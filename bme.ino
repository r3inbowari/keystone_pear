#include "apds.h" // include wire i2c
#include "bme.h"

float hum = 0;
float pre = 0;
float alt = 0;
float tem = 0;

Adafruit_BME280 bme;

void init_bme() {
  Serial.println(F("--------------------------------"));
  Serial.println(F("SparkFun BME-280 - HATPSensor"));
  Serial.println(F("--------------------------------"));
  if (! bme.begin(0x76, &Wire)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  Serial.println(F("BME sensor is now running"));
  Serial.println(F("--------------------------------"));
}

void bme_gather() {
  hum = bme.readHumidity();
  pre = bme.readPressure();
  alt = bme.readAltitude(SEALEVELPRESSURE_HPA);
  tem = bme.readTemperature();
  Serial.print("[BME] ");
  Serial.print("Hum: ");
  Serial.print(hum);
  Serial.print(" Pre: ");
  Serial.print(pre);
  Serial.print(" Alt: ");
  Serial.print(alt);
  Serial.print(" Tem: ");
  Serial.println(tem);

}
