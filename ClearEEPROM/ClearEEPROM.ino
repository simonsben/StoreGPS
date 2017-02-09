#include<EEPROM.h>

void setup() {
  EEPROM.write(0, 0);
  Serial.begin(9600);
  Serial.println("Cleared.");
}

void loop() {
  return;
}
