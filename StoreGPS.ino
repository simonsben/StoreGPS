#include <EEPROM.h>
#include "Adafruit_GPS.h"
#include<SoftwareSerial.h>

//Define Global constants and variables----------------------------------------
#define saveSwitch 3
#define LEDPin 13

// GPS constants
#define MAXLINELENGTH 120
//define newSerial Serial
#define GPS_BAUD 9600
Adafruit_GPS GPS;
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
SoftwareSerial newSerial(5, 6);

//Declaring global variables
float lat = 0, lng = 0; //Example float value --> to be replaced
long last, count = 0; //
int prev, state = -(digitalRead(saveSwitch)-1);

//Setup code--------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  pinMode(LEDPin, OUTPUT); //Assign pin mode for LED
  pinMode(saveSwitch, INPUT_PULLUP); //Set pin mode for button 

  delay(4000); //Delay to allow GPS to start
  setupGPS(); //GPS setup commands (configures GPS)
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
      else if(tme > 2000) {
        Serial.println("Long");
        longpress();
      }
      else {
        Serial.println("Short");
        saveData();
      }
    }
  }
  getGPSLocation();

  delay(50);
}

//Long press action--------------------------------------------------------------
//Flash the current number of saved data points
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
}

//Confirm record-----------------------------------------------------------------
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

//Update GPS location------------------------------------------------------------
void updateloc() {
  //getGPSLocation();
  lng = GPS.longitude;
  lat = GPS.latitude;
  Serial.println("Current location");
  Serial.print("(Long, Lat) = (");
  Serial.print(lng);
  Serial.print(", ");
  Serial.print(lat);
  Serial.println(")");
  Serial.print("GPS.time = ");
  Serial.println(GPS.milliseconds);
}

//Save GPS Function--------------------------------------------------------------
//Save current GPS location to EEPROM
void saveData() {
  updateloc();
  unsigned char* pt = (unsigned char*) &lng;
  int i, len;
  len = EEPROM.read(0);
  
  for(i=0;i<4;i++)
    EEPROM.write(i + len*8 + 2, pt[i]);
  //Serial.println(lng, 4);
  
  pt = (unsigned char*) &lat;
  for(i=0;i<4;i++)
    EEPROM.write(i + len*8 + 6, pt[i]);
  //Serial.println(lat, 4);
  
  EEPROM.write(0, len + 1);
  
  confirm();
}

//Clear current datapoints-------------------------------------------------------
void clearmem() {
  EEPROM.write(0, 0);
  digitalWrite(LEDPin, HIGH);
  delay(5000);
  digitalWrite(LEDPin, LOW);
}

//Initialize GPS (From Communicator.h in team code)-------------------------------
void setupGPS()  {
  // Initialize the variables in GPS class object
  GPS.init();
  
  // Start the serial communication
  newSerial.begin(GPS_BAUD);

  // Commands to configure GPS:
  newSerial.println(PMTK_SET_NMEA_OUTPUT_RMCONLY);     // Set to only output GPRMC (has all the info we need),
  newSerial.println(SET_NMEA_UPDATE_RATE_5HZ);     // Increase rate strings sent over serial
  newSerial.println(PMTK_API_SET_FIX_CTL_5HZ);     // Increase rate GPS 'connects' and syncs with satellites
  newSerial.println(ENABLE_SBAB_SATELLITES);       // Enable using a more accurate type of satellite
  newSerial.println(ENABLE_USING_WAAS_WITH_SBSB_SATS);   // Enable the above satellites in 'fix' mode (I think)
  delay(3000);  //Not really sure if needed.
  
  // Flush the GPS input (still unsure if the GPS sends response to the above commands)
  int num = 0;
  while (newSerial.available() && num++ < 50) {
    //newSerial.read();
    DEBUG_PRINT("FLUSH RESPONSE: ");
    DEBUG_PRINT(newSerial.read());
    DEBUG_PRINTLN("");
  }

}

void getGPSLocation() {

  //DEBUG_PRINT("Bytes available: ");
  //DEBUG_PRINTLN(newSerial.read());
  while (newSerial.available()) {

    nmeaBuf[nmeaBufInd] = newSerial.read();
    DEBUG_PRINT(nmeaBuf[nmeaBufInd]);
    
    if (nmeaBuf[nmeaBufInd++] == '\n') { // Increment index after checking if current character signifies the end of a string
      nmeaBuf[nmeaBufInd - 1] = '\0'; // Add null terminating character (note: -1 is because nmeaBufInd is incremented in if statement)
      newParsedData = GPS.parse(nmeaBuf);   // This parses the string, and updates the values of GPS.lattitude, GPS.longitude etc.
      nmeaBufInd = 0;  // Regardless of it parsing sucessful, we want to reset position back to zero
    }

    if (nmeaBufInd >= MAXLINELENGTH) { // Should never happen. Means a corrupted packed and the newline was missed. Good to have just in case
      nmeaBufInd = 0;  // Note the next packet will then have been corrupted as well. Can't really recover until the next-next packet
    }
    //DEBUG_PRINTLN("");

  }
}

