#include <EEPROM.h>

//Define Global constants and variables
#define saveSwitch 3
float b = 123.4567;
long last;

void setup() {
  int i;
  unsigned char* pt = (unsigned char*) &b;
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  
  //for(i=0;i<4;i++)
  //  EEPROM.write(i, pt[i]);
  
  pinMode(saveSwitch, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(saveSwitch), prnt, RISING);
}

void loop() {
  int i;
  float a = 123.4567;
  unsigned char* pt;
  unsigned char* gt;

  delay(4000);

  //pt = (unsigned char*) &a;
  //gt = (unsigned char*) &b;

  //for(i=0;i<4;i++)
  //  EEPROM.write(i, pt[i]);

  
  
  //Serial.println(b, 4);

  /*
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);              // wait for a second
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);  
  */
  /*
  for(i=0;i<a;i++) {
    digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(1000);              // wait for a second
    digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
    delay(1000);    
  } */

}

void prnt() {
  float str;
  unsigned char* gt = (unsigned char*) &str;
  int i;
  
  for(i=0;i<4;i++)
    gt[i] = EEPROM.read(i);

  if(!digitalRead(saveSwitch) && millis()-last > 1000) {
    Serial.println(millis(), 2);
    Serial.println(str, 4);
    last = millis();
  }
  digitalWrite(13, !digitalRead(saveSwitch));
}

void count() {
  
}

