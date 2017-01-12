#include <EEPROM.h>
#include "Adafruit_GPS.h"

//Define Global constants and variables----------------------------------------
#define saveSwitch 3
#define LEDPin 13

// GPS constants
#define MAXLINELENGTH 120
#define GPS_SERIAL Serial
#define GPS_BAUD 9600
Adafruit_GPS GPS;

#define DEBUG_COMMUNICATOR
#ifdef DEBUG_COMMUNICATOR
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

//gps variables and functions
char nmeaBuf[MAXLINELENGTH];
int nmeaBufInd = 0;
boolean newParsedData = false;

float lat = 0, lng = 0; //Example float value --> to be replaced
long last, count = 0; //
int prev, state = -(digitalRead(saveSwitch)-1);

//Pin setup---------------------------------------------------------------------
void setup() {
  int i;
  Serial.begin(9600);
  pinMode(LEDPin, OUTPUT); //Assign pin mode for LED

  //Initialize button using interupt pin
  pinMode(saveSwitch, INPUT_PULLUP);
  setupGPS();
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

//Update GPS location------------------------------------------------------------
void updateloc() {
  getGPSLocation();
  lng = GPS.longitude;
  lat = GPS.latitude;
  Serial.println("Update location");
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

//Initialize GPS (From Communicator.h in team code)-------------------------------
void setupGPS()  {
  // Initialize the variables in GPS class object
  GPS.init();

  // Start the serial communication
  GPS_SERIAL.begin(GPS_BAUD);

  // Commands to configure GPS:
  GPS_SERIAL.println(PMTK_SET_NMEA_OUTPUT_RMCONLY);     // Set to only output GPRMC (has all the info we need),
  GPS_SERIAL.println(SET_NMEA_UPDATE_RATE_5HZ);     // Increase rate strings sent over serial
  GPS_SERIAL.println(PMTK_API_SET_FIX_CTL_5HZ);     // Increase rate GPS 'connects' and syncs with satellites
  GPS_SERIAL.println(ENABLE_SBAB_SATELLITES);       // Enable using a more accurate type of satellite
  GPS_SERIAL.println(ENABLE_USING_WAAS_WITH_SBSB_SATS);   // Enable the above satellites in 'fix' mode (I think)
  delay(3000);  //Not really sure if needed.

  // Flush the GPS input (still unsure if the GPS sends response to the above commands)
  while (GPS_SERIAL.available()) {
    //GPS_SERIAL.read();
    DEBUG_PRINT("FLUSH RESPONSE: ");
    DEBUG_PRINT(GPS_SERIAL.read());
    DEBUG_PRINTLN("");
  }

}

void getGPSLocation() {
  while (GPS_SERIAL.available()) {

    nmeaBuf[nmeaBufInd] = GPS_SERIAL.read();
    //DEBUG_PRINT(nmeaBuf[nmeaBufInd]);
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

