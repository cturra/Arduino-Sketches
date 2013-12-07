/*
----------------------------------------------------

           PhotoR     10K
 +5    o---/\/\/-----/\/\/---o GND


----------------------------------------------------
*/

#include <Wire.h>
#include <DateTime.h>
#include <RTClib.h>

int LOW_LIGHT   = 250; //define low light threashold
int SLEEP_SEC   = 60;  //number of seconds to sleep between polling
int HOUR_OFF    = 22;  //hour (24hr) of the day to turn lights off 
int MINUTE_OFF  = 30;  //minutes past the hour to turn lights off 
int MIN_HOUR_ON = 16;  //earliest hour (24hr) of the day to turn lights on

int LIGHT_PIN = 0;    //define a pin for Photo resistor
int POWER_PIN = 7;    //define a ping for PowerSwitch Tail
boolean DEBUG = true; //debug output to serial monitor

int POWER_ON_HOUR = 0;
int POWER_ON_DAY  = 0;

RTC_Millis rtc;

void setup()
{
  pinMode(POWER_PIN, OUTPUT); 
  rtc.begin(DateTime(__DATE__, __TIME__));
  Serial.begin(9600);  //Begin serial communcation
}

void loop()
{
  DateTime now = rtc.now();
  
  if (DEBUG) {
    Serial.print( "["+String(now.hour())+":"+String(now.minute())+"] " );
    Serial.println( "Current light reading: "+String(analogRead(LIGHT_PIN)) );
    
    if ( POWER_ON_HOUR > 0 ) {
      Serial.println(" => lights are currently on.");
    }
    else {
      Serial.println(" => lights are currently off.");
    }
  }
  
  // check if light level is low and lights are not currently turned on
  if ( ((int)analogRead(LIGHT_PIN) < LOW_LIGHT) && (POWER_ON_HOUR <= 0) ) {
    // confirm it's late enough in the day to turn on the lights
    if ( (int)now.hour() >= MIN_HOUR_ON ){
      // confirm the lights haven't been on already today!
      if ( (int)now.day() != POWER_ON_DAY ) {
        powerOn();
        POWER_ON_HOUR = (int)now.hour();
        POWER_ON_DAY  = (int)now.day();
      
        if (DEBUG) {
          Serial.println(" => turning lights on.");
        }
      }
      else {
        if (DEBUG) {
          Serial.println(" => not turning lights on b/c they've been on today.");
        }
      }
    }
    else {
      if (DEBUG) {
        Serial.println(" => not turning lights on b/c it's too early (see: MIN_HOUR_ON).");
      }
    }
  }
  
  if ( POWER_ON_HOUR > 0 ) {
    //lights are on. 
    if ( ((int)now.hour() >= HOUR_OFF) && ((int)now.minute() >= MINUTE_OFF) ) {
      powerOff();
      POWER_ON_HOUR = 0;
      
      if (DEBUG) {
        Serial.println(" => turning lights off for the night.");
      }
    }
  }
  
  // sleep until the next polling interval
  delay(((long)SLEEP_SEC*1000)); 
}

void powerOn() {
  digitalWrite(POWER_PIN, HIGH);
}

void powerOff() {
  digitalWrite(POWER_PIN, LOW);
}


