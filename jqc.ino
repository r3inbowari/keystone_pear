#include "jqc.h"

bool status_coupler0 = false;
bool status_coupler1 = false;

void init_jqc() {
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
}

void turn_on_coupler(int coupler_member) {
  if (coupler_member == 0) {
    status_coupler0 = true;
    digitalWrite(GPIO_COUPLER0, HIGH);
  } else if (coupler_member == 1) {
    status_coupler1 = true;
    digitalWrite(GPIO_COUPLER1, HIGH);
  }
}

void turn_off_coupler(int coupler_member) {
  if (coupler_member == 0) {
    status_coupler0 = false;
    digitalWrite(GPIO_COUPLER0, LOW);
  } else if (coupler_member == 1) {
    status_coupler1 = false;
    digitalWrite(GPIO_COUPLER1, LOW);
  }
}

void reset_all_couplers() {
  digitalWrite(GPIO_COUPLER0, LOW);
  digitalWrite(GPIO_COUPLER1, LOW);
  status_coupler0 = false;
  status_coupler1 = false;
}
