#include "utils.h"

const int const_length = 50;

void LogDivisionSta(String msg) {
  int len = const_length - msg.length();
  String prefix_hs = "";
  for (int i = 0; i < len / 2; i++) {
    prefix_hs += "-";
  }
  Serial.println(prefix_hs + " " + msg + " STA " + prefix_hs);
}

void LogDivisionEnd(String msg) {
  int len = const_length - msg.length();
  String prefix_hs = "";
  for (int i = 0; i < len / 2; i++) {
    prefix_hs += "-";
  }
  Serial.println(prefix_hs + " " + msg + " END " + prefix_hs);
  Serial.println("");
  Serial.println("");
}

IPAddress getlocalIP() {
  return IPAddress(mesh.getStationIP());
}


bool checkIpv4(const char *ip) {
  Serial.print("check ipv4：");
  Serial.println(ip);
  if (NULL == ip) {
    return false;
  }
  const char *q = ip;
  unsigned short int s = 0, count = 0, digitNumber = 0;
  bool hasZero = false;
  while (' ' == *q) {
    q++;
  }
  while ('\0' != *q) {
    if ('.' == *q) {
      if (digitNumber == 0) {
        return false;
      }
      s = 0;
      digitNumber = 0;
      count++;
      hasZero = false;
      q++;
      continue;
    }
    if (*q < '0' || *q > '9') {
      if (' ' == *q && 3 == count) {
        const char *qq = q;
        while (' ' == *qq) {
          qq++;
        }
        return '\0' == *qq;
      } else {
        return false;
      }
    }
    int x = *q - '0';
    s = s * 10 + x;
    if (0 == s) {
      if (hasZero) {
        return false;
      } else {
        hasZero = true;
      }
    }
    if (s > 255) {
      return false;
    }
    digitNumber++;
    q++;
  }
  return (3 == count);
}

/*
   © Francesco Potortì 2013 - GPLv3 - Revision: 1.13

   Send an NTP packet and wait for the response, return the Unix time

   To lower the memory footprint, no buffers are allocated for sending
   and receiving the NTP packets.  Four bytes of memory are allocated
   for transmision, the rest is random garbage collected from the data
   memory segment, and the received packet is read one byte at a time.
   The Unix time is returned, that is, seconds from 1970-01-01T00:00.
*/
unsigned long inline ntpUnixTime (UDP &udp)
{
  static int udpInited = udp.begin(123);
  const char timeServer[] = "ntp.aliyun.com";
  const long ntpFirstFourBytes = 0xEC0600E3;
  if (! udpInited)
    return 0;
  udp.flush();
  if (! (udp.beginPacket(timeServer, 123)
         && udp.write((byte *)&ntpFirstFourBytes, 48) == 48
         && udp.endPacket()))
    return 0;
  const int pollIntv = 150;
  const byte maxPoll = 15;
  int pktLen;
  for (byte i = 0; i < maxPoll; i++) {
    if ((pktLen = udp.parsePacket()) == 48)
      break;
    delay(pollIntv);
  }
  if (pktLen != 48)
    return 0;
  const byte useless = 40;
  for (byte i = 0; i < useless; ++i)
    udp.read();
  unsigned long time = udp.read();
  for (byte i = 1; i < 4; i++)
    time = time << 8 | udp.read();
  time += (udp.read() > 115 - pollIntv / 8);
  udp.flush();
  return time - 2208988800ul;
}

void INFO(String msg) {
  Serial.println("[INFO] " + msg);
}

void INFOL(String msg) {
  Serial.print("[INFO] " + msg);
}

void INFOM(String msg) {
  Serial.print(msg);
}

void INFOH(String msg) {
  Serial.println(msg);
}

void WARN(String msg) {
  Serial.println("[WARN] " + msg);
}

void WARNL(String msg) {
  Serial.print("[WARN] " + msg);
}

void WARNM(String msg) {
  Serial.print(msg);
}

void WARNH(String msg) {
  Serial.println(msg);
}
