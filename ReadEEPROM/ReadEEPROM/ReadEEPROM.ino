#include<EEPROM.h>
float lat, lng;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  readData();
}

void loop() {
  // put your main code here, to run repeatedly:
  
}

void readData() {
  unsigned char* pt = (unsigned char*) &lng;
  int i, j, len;
  len = EEPROM.read(1);

  for(j=0;j<len;j++) {
    for(i=0;i<4;i++)
      pt[i] = EEPROM.read(i + len*4 + 2);
    Serial.println(lng, 4);
    
    pt = (unsigned char*) &lat;
    for(i=0;i<4;i++)
      pt[i] = EEPROM.read(i + len*4 + 6);
    Serial.println(lat, 4);
  }
}
