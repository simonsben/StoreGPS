//#include "Adafruit_GPS.h"
#include "Communicator.h"

//Define Global constants and variables----------------------------------------
#define saveSwitch 3
#define LEDPin 13

// GPS constants
#define MAXLINELENGTH 120
//define newSerial Serial
#define GPS_BAUD 9600
//Adafruit_GPS GPS;
Communicator comm;
char nmeaBuf[MAXLINELENGTH];
int nmeaBufInd = 0;
boolean newParsedData = false;

#define DEBUG_COMMUNICATOR
#ifdef DEBUG_COMMUNICATOR
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

//Secondary serial connection allows Uno to maintain a serial connection to a computer at the same time as the GPS
//SoftwareSerial newSerial(5, 6);

//Declaring global variables
float lat = 0, lng = 0; //Example float value --> to be replaced
long last, count = 0; //
int prev, state = -(digitalRead(saveSwitch)-1);

//Confirm send------------------------------------------------------------------
//Confirmation flash to acknowledge save of location
void confirm() { 
  int i;
  delay(300);
  for(i=0;i<2;i++) {
    digitalWrite(LEDPin, HIGH);
    delay(250);
    digitalWrite(LEDPin, LOW);
    delay(150);
  }
}

//Send GPS Function--------------------------------------------------------------
//Calls on communicator to refresh values and send via Xbee
void sendData() {
  comm.sendData(); 
  confirm();
}

//Clear current datapoints-------------------------------------------------------
void clearmem() {
  char mess = 'c';
  comm.sendMessage(mess);
}

//Setup code--------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  pinMode(LEDPin, OUTPUT); //Assign pin mode for LED
  pinMode(saveSwitch, INPUT_PULLUP); //Set pin mode for button 

  delay(4000); //Delay to allow GPS to start
  comm.initialize();
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
      //else if(tme > 2000) {
      //  Serial.println("Long");
      //  longpress();
      //}
      else {
        Serial.println("Short");
        sendData();
      }
    }
  }

  delay(50);
}

//Long press action--------------------------------------------------------------
//Flash the current number of saved data points
//No longer used as program re-written to send data instead
/*
void longpress()  {
  int i, count; //Declare counter and read number of data entries
  delay(750); //Allow time to look at LED after button press
  count = EEPROM.read(1);
  
  for(i=0;i<count;i++)  {
    digitalWrite(LEDPin, HIGH);
    delay(1000);
    digitalWrite(LEDPin, LOW);
    delay(1000);
  }
  
  Serial.print("Itterations: ");
  Serial.println(count);
} */



