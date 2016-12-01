#include <EEPROM.h>

//Define Global constants and variables
#define saveSwitch 3
#define LEDPin 13
float lat = 123.4567, lng = 765.4321; //Example float value --> to be replaced
long last, count = 0; //

void setup() {
  int i;
  Serial.begin(9600);
  pinMode(LEDPin, OUTPUT); //Assign pin mode for LED

  //Initialize button using interupt pin
  pinMode(saveSwitch, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(saveSwitch), measure, RISING);
}

void loop() {

  delay(4000);
}



/*
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
*/
//Count duration of key press
long cnt() {
  long srt = millis(); //Declare reference time
  while(digitalRead(saveSwitch)) //Check whether switch is down
    delay(25);

  return (millis() - srt);
}


void measure() {
  if(millis()-last < 1000)
    return;
  long curr = millis();

  while(millis() - curr < 20) {
    while(digitalRead(saveSwitch)) //Check whether switch is down
      delay(25);

    if(millis() - curr < 20)  {
      if(!digitalRead(saveSwitch))
        return;
      curr = millis();
    }
  }
    
  //Decide whether a long or short press was made
  if(millis()-curr > 750)
    longpress();
  else {
    Serial.println("Saving data");
    saveData();
    last = millis();
  }

}

//Flash the current number of saved data points
void longpress()  {
  int i;
  for(i=0;i<count;i++)  {
    digitalWrite(LEDPin, HIGH);
    delay(1000);
    digitalWrite(LEDPin, LOW);
    delay(1000);
  }
  Serial.print("Itterations: ");
  Serial.println(count);
}

//Save current GPS location to EEPROM
void saveData() {
  unsigned char* pt = (unsigned char*) &lng;
  int i;
  for(i=0;i<4;i++)
    EEPROM.write(i + count*8, pt[i]);
  Serial.println(lng, 4);
  
  pt = (unsigned char*) &lat;
  for(i=0;i<4;i++)
    EEPROM.write(i + count*8 + 4, pt[i]);
  Serial.println(lat, 4);
  
  confirm();
}

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

