/**
   utils behavior header
   @author r3inbowari
   @date 2020/9/12
*/

#ifndef __UTILS_H
#define __UTILS_H
#include "mesh.h"
#include <Time.h>
#include <TimeLib.h>

unsigned long inline ntpUnixTime (UDP &udp);
void LogDivisionSta(String msg);
void LogDivisionEnd(String msg);
bool checkIpv4(const char *ip);
IPAddress getlocalIP();
#endif
