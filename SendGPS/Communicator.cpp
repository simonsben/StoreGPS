/*

  This class deals with all serial communication. This includes sending and receiving data from the XBee, as well as sending commands and receiving
  data from the GPS. The GPS class is only called from here
  It also controls the servos for the dropbay, since the "drop payload" command will be received directly by this class and it's easier then having another class.

*/

//#include "Arduino.h"
#include "Communicator.h"
#include <Servo.h>

// During testing we might want to send over USB to computer. Instead of commenting out a lot of  'SerialUSB.print(...)' statements we can define a macro as below
// If the line directly below is NOT commented out, then DEGUB_PRINT(...) will send to computer. If it is commented out, the macro DEBUG_PRINT/LN will be empty and
// the compiler will optimize it out of the code automatically
#define DEBUG_COMMUNICATOR

#ifdef DEBUG_COMMUNICATOR
#define DEBUG_PRINT(x) SerialUSB.print(x)
#define DEBUG_PRINTLN(x) SerialUSB.println(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#endif

Adafruit_GPS GPS;

//Constructor
Communicator::Communicator() {}
Communicator::~Communicator() {}

// Function called in void setup() that instantiates all the variables, attaches pins, ect
// This funciton needs to be called before anything else will work
void Communicator::initialize() {

  pinMode(TX_TO_DISCONNECT, INPUT);  // Ensure it's in high impedance state
  pinMode(RX_TO_DISCONNECT, INPUT);  // Ensure it's in high impedance state

#ifdef DEBUG_COMMUNICATOR
  SerialUSB.begin(SERIAL_USB_BAUD); // This is to computer
  DEBUG_PRINTLN("at start of comm initialize");
#endif

  // Set initial values to 0
  altitude = 0;

  // Initialize serial commuication to Xbee.  
  XBEE_SERIAL.begin(XBEE_BAUD);  //this is to Xbee
  setupGPS();
}

void Communicator::setupGPS() {

  // Initialize the variables in GPS class object
  GPS.init();

  // Start the serial communication
  GPS_SERIAL.begin(GPS_BAUD);

  // Commands to configure GPS:
  GPS_SERIAL.println(PMTK_SET_NMEA_OUTPUT_RMCONLY); 		// Set to only output GPRMC (has all the info we need),
  GPS_SERIAL.println(SET_NMEA_UPDATE_RATE_5HZ);			// Increase rate strings sent over serial
  GPS_SERIAL.println(PMTK_API_SET_FIX_CTL_5HZ);			// Increase rate GPS 'connects' and syncs with satellites
  GPS_SERIAL.println(ENABLE_SBAB_SATELLITES);				// Enable using a more accurate type of satellite
  GPS_SERIAL.println(ENABLE_USING_WAAS_WITH_SBSB_SATS); 	// Enable the above satellites in 'fix' mode (I think)
  delay(3000);  //Not really sure if needed.

  // Flush the GPS input (still unsure if the GPS sends response to the above commands)
  while (GPS_SERIAL.available()) {
    //GPS_SERIAL.read();
    DEBUG_PRINT("FLUSH RESPONSE: ");
    DEBUG_PRINT(GPS_SERIAL.read());
    DEBUG_PRINTLN("");
  }

  DEBUG_PRINTLN("DONE FLUSHING");

}

 
void Communicator::getSerialDataFromGPS() {

  while (GPS_SERIAL.available()) {

    nmeaBuf[nmeaBufInd] = GPS_SERIAL.read();
    //DEBUG_PRINT(nmeaBuf[nmeaBufInd]);
    if (nmeaBuf[nmeaBufInd++] == '\n') { // Increment index after checking if current character signifies the end of a string
      nmeaBuf[nmeaBufInd - 1] = '\0'; // Add null terminating character (note: -1 is because nmeaBufInd is incremented in if statement)
      newParsedData = GPS.parse(nmeaBuf); 	// This parses the string, and updates the values of GPS.lattitude, GPS.longitude etc.
      nmeaBufInd = 0;  // Regardless of it parsing sucessful, we want to reset position back to zero
    }

    if (nmeaBufInd >= MAXLINELENGTH) { // Should never happen. Means a corrupted packed and the newline was missed. Good to have just in case
      nmeaBufInd = 0;  // Note the next packet will then have been corrupted as well. Can't really recover until the next-next packet
    }
    //DEBUG_PRINTLN("");

  }
}

// Data is sent via wireless serial link to ground station
// data packet format:  *p%ALTITUDE%LATTITUDE%LONGITUDE%secondee
// Not anymore: now is bytewise transmission of floats
// Total number of bytes: 11 (*p% type) + ~ 50 (data) = 61 bytes *4x/second = 244 bytes/s.  Each transmission is under outgoing buffer (128 bytes) and baud
// No other serial communication can be done in other classes!!!
void Communicator::sendData() {

  // Change to:  (total of 28? bytes)
  XBEE_SERIAL.print("*p");
  sendFloat((float)altitude);
  sendFloat(GPS.latitude);
  sendFloat(GPS.longitude);
  sendUint8_t(GPS.seconds);
  XBEE_SERIAL.print("ee");

}

void Communicator::sendMessage(char message) {
  XBEE_SERIAL.print("*");
  XBEE_SERIAL.print(message);
  XBEE_SERIAL.print("ee");
}

void Communicator::sendUint8_t(uint8_t toSend) {
  byte *data = (byte*)&toSend; //cast address of input to byte array
  XBEE_SERIAL.write(data, sizeof(toSend));
}


void Communicator::sendFloat(float toSend) {
  byte *data = (byte*)&toSend; //cast address of float to byte array
  XBEE_SERIAL.write(data, sizeof(toSend));  //send float as 4 bytes
}
