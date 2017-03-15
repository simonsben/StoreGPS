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
  Serial.println("Begin setup");
  delay(4000); //Delay to allow GPS to start
  setupGPS(); //GPS setup commands (configures GPS)
}

//Main loop---------------------------------------------------------------------
void loop() {
  getGPSLocation();
  //printToSerial();
  
  delay(50);
}

void printToSerial() {
  Serial.print("Fix quality: ");
  Serial.print(GPS.fixquality);
  Serial.print(" Satellites: ");
  Serial.print(GPS.satellites);
  Serial.print(" HDOP: ");
  Serial.print(GPS.HDOP);
  Serial.print(" Speed: ");
  Serial.println(GPS.speed);
}

//Initialize GPS (From Communicator.h in team code)-------------------------------
void setupGPS()  {
  // Initialize the variables in GPS class object
  GPS.init();
  
  // Start the serial communication
  newSerial.begin(GPS_BAUD);
  
  // Commands to configure GPS:
  newSerial.println(PMTK_SET_NMEA_UPDATE_0HZ);
  delay(500);
  flushAndPrint();
  
  Serial.println("Begin setting GPS");
  newSerial.println(PMTK_SET_NMEA_UPDATE_0HZ);
  flushAndPrint();
  
  newSerial.println(PMTK_SET_NMEA_OUTPUT_RMCGGA);     // Set to only output GPRMC (has all the info we need),
  flushAndPrint();
  
  //newSerial.println(SET_NMEA_UPDATE_RATE_5HZ);     // Increase rate strings sent over serial
  //flushAndPrint();
  
  newSerial.println(PMTK_API_SET_FIX_CTL_5HZ);     // Increase rate GPS 'connects' and syncs with satellites
  flushAndPrint();
  
  newSerial.println(ENABLE_SBAB_SATELLITES);       // Enable using a more accurate type of satellite
  flushAndPrint();
  
  newSerial.println(ENABLE_USING_WAAS_WITH_SBSB_SATS);   // Enable the above satellites in 'fix' mode (I think)
  flushAndPrint();

  newSerial.println(PMTK_SET_NMEA_UPDATE_5HZ);
  flushAndPrint();
  
  //delay(3000);  //Not really sure if needed.
  
  // Flush the GPS input (still unsure if the GPS sends response to the above commands)
  int num = 0;
  /*
  while (newSerial.available() && num++ < 50) {
    //newSerial.read();
    DEBUG_PRINT("FLUSH RESPONSE: ");
    DEBUG_PRINT(newSerial.read());
    DEBUG_PRINTLN("");
  } */
  

}

void flushAndPrint() {
  delay(200);
  char hold;
  int numBytes = newSerial.available();
  for(int i=0;i<numBytes;i++)  {
    Serial.print((char)newSerial.read());
  }
  Serial.print("overflow: ");
  Serial.println(newSerial.overflow());
}

//Parse new GPS data when availible -----------------------------------------------
void getGPSLocation() {
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
      DEBUG_PRINTLN("");
    }
    
  }
}

