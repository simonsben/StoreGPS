#include <EEPROM.h>

//Define Global constants and variables----------------------------------------
#define saveSwitch 3
#define LEDPin 13
float lat = 123.4567, lng = 765.4321; //Example float value --> to be replaced
long last, count = 0; //
int prev, state = -(digitalRead(saveSwitch)-1);

//Pin setup---------------------------------------------------------------------
void setup() {
  int i;
  Serial.begin(9600);
  pinMode(LEDPin, OUTPUT); //Assign pin mode for LED

  //Initialize button using interupt pin
  pinMode(saveSwitch, INPUT_PULLUP);
}

//Main loop---------------------------------------------------------------------
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

      if(tme > 5000) {
        Serial.println("Cleared.");
        clearmem();
      }
      if(tme > 2000) {
        Serial.println("Long");
        longpress();
      }
      else {
        Serial.println("Short");
        saveData();
      }
    }
  }

  delay(50);
}

//Long press action--------------------------------------------------------------
//Flash the current number of saved data points
void longpress()  {
  int i, count; //Declare counter and read number of data entries
  delay(250); //Allow time to look at LED after button press
  count = EEPROM.read(1);
  
  for(i=0;i<count;i++)  {
    digitalWrite(LEDPin, HIGH);
    delay(1000);
    digitalWrite(LEDPin, LOW);
    delay(1000);
  }
  
  Serial.print("Itterations: ");
  Serial.println(count);
}

//Confirm record-----------------------------------------------------------------
//Confirmation flash to acknowledge save of location
void confirm() { 
  int i;
  for(i=0;i<2;i++) {
    digitalWrite(LEDPin, HIGH);
    delay(250);
    digitalWrite(LEDPin, LOW);
    delay(400);
  }
}

//Save GPS Function--------------------------------------------------------------
//Save current GPS location to EEPROM
void saveData() {
  unsigned char* pt = (unsigned char*) &lng;
  int i, len;
  len = EEPROM.read(1);
  
  for(i=0;i<4;i++)
    EEPROM.write(i + len*4 + 2, pt[i]);
  Serial.println(lng, 4);
  
  pt = (unsigned char*) &lat;
  for(i=0;i<4;i++)
    EEPROM.write(i + len*4 + 6, pt[i]);
  Serial.println(lat, 4);
  
  EEPROM.write(1, len + 1);
  
  confirm();
}

//Clear current datapoints-------------------------------------------------------
void clearmem() {
  EEPROM.write(1, 0);
  digitalWrite(LEDPin, HIGH);
  delay(5000);
  digitalWrite(LEDPin, LOW);
}

