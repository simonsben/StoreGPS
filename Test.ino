#include <EEPROM.h>

//Define Global constants and variables
#define saveSwitch 3
#define LEDPin 13
float lat = 123.4567, lng = 765.4321; //Example float value --> to be replaced
long last, count = 0; //
int prev, state = -(digitalRead(saveSwitch)-1);

void setup() {
  int i;
  Serial.begin(9600);
  pinMode(LEDPin, OUTPUT); //Assign pin mode for LED

  //Initialize button using interupt pin
  pinMode(saveSwitch, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(saveSwitch), measure, RISING);
}

void loop() {
  long tme;
  prev = state;
  state = -(digitalRead(saveSwitch)-1);
  
  if(state != prev) {
    if(state == 1) {
      Serial.println("UP!");
      tme = millis();
      while(digitalRead(saveSwitch) == 0)
        delay(40);
      tme = millis() - tme;
      Serial.print(tme);
      Serial.println(" - Down.");
    
      if(tme > 2000)
        Serial.println("Long");
      else
        Serial.println("Short");
    }
  }   
 
  delay(50);
}


void measure() {
  long tme;
  prev = state;
  state = -(digitalRead(saveSwitch)-1);
  
  if(state != prev) {
    if(state == 1) {
      Serial.println("UP!");
      tme = millis();
      while(digitalRead(saveSwitch) == 0)
        delay(40);
      tme = millis() - tme;
      Serial.print(tme);
      Serial.println(" - Down.");
    
      if(tme > 2000)
        Serial.println("Long");
      else
        Serial.println("Short");
    }
  }  
}
