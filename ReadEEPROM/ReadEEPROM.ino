#include<EEPROM.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  readData();
  Serial.println("Data read.");
}

void loop() {
  return;
}

void readData() {
  float lat, lng;
  byte* pt;
  int i, j;
  byte len;
  
  len = EEPROM.read(0);
  Serial.print("Number of points: ");
  Serial.println(len);

  for(j=0;j<len;j++) {
    pt =  (byte*) &lng;
    for(i=0;i<4;i++)
      pt[i] = EEPROM.read(i + j*8 + 2);
    Serial.print("Long: ");
    Serial.println(lng, 4);
    
    pt = (byte*) &lat;
    for(i=0;i<4;i++)
      pt[i] = EEPROM.read(i + j*8 + 6);
    Serial.print("Lat: ");
    Serial.println(lat, 4);
  }
}
