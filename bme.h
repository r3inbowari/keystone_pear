#ifndef __BME_H
#define __BME_H

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define SEALEVELPRESSURE_HPA (1013.25)

extern float hum;
extern float pre;
extern float alt;
extern float tem;

#endif
