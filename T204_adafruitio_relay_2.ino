//**************************************************************************
// T204
// Relay sensor values to Adafruit IO
// Running on Adafruit ESP32 Feather
// Connected to RFM69 via an Arduino Pro Mini
//
// Original Adafruit IO code by:
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016 Adafruit Industries
// Licensed under the MIT license.
// All text above must be included in any redistribution.

/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#include "config.h"
#include <HardwareSerial.h>

/****************************************************************************/

String inputString = "";         // a String to hold incoming data
bool stringComplete = false;     // whether the string is complete
long int last_io_run;

HardwareSerial Serial1(1);


// Adafruit IO Feeds at Villa Astrid
AdafruitIO_Feed *water_temp = io.feed("villaastrid.water-temp");
AdafruitIO_Feed *dock_temp = io.feed("villaastrid.dock-temp");
AdafruitIO_Feed *outdoor1_temp = io.feed("villaastrid.outdoor1-temp");

void setup() {

  // start the serial connection
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1,RX,TX);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);

  // wait for serial monitor to open
  //while(! Serial);
  delay(3000);
  Serial.println("T204  Villa Astrid radio messages relayed to Adafruit IO");
  Serial.print("Connecting to Adafruit IO");

  // connect to io.adafruit.com
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());
  last_io_run = millis();
}

String parse_json(String fromStr, char *tag){
   int pos1;
   int pos2;
   //Serial.println(fromStr); Serial.println(tag);
   pos1 = fromStr.indexOf(tag);
   if (tag == "Value"){
      pos1 = fromStr.indexOf(":",pos1) +1;
      pos2 = fromStr.indexOf(",",pos1);
  }
   else{
      pos1 = fromStr.indexOf(":\"",pos1) +2;
      pos2 = fromStr.indexOf("\"",pos1);
   }
    return(fromStr.substring(pos1,pos2));
}


void loop() {
  String sensor_zone;
  String sensor_name;
  String sensor_value;
  
  
  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  if ((millis() - last_io_run ) > 5000 ){
      io.run();
      last_io_run = millis();
  }
 
  read_serial();
  if (stringComplete) {  
    Serial.println(inputString);
    sensor_zone = parse_json(inputString,"Zone");
    sensor_name = parse_json(inputString,"Sensor");
    sensor_value = parse_json(inputString,"Value");
    Serial.println(sensor_zone); Serial.println(sensor_name); Serial.println(sensor_value);
    if (sensor_zone == "Dock"){
      if (sensor_name == "T_Water") water_temp->save(sensor_value);
      if (sensor_name == "T_bmp180") dock_temp->save(sensor_value);
    }
    if (sensor_zone == "OD_1"){
       if (sensor_name == "Temp") outdoor1_temp->save(sensor_value);     
    }
   // clear the string:
    inputString = "";
    stringComplete = false;
  }
  
  
 
}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/


void read_serial() {
  while (Serial1.available()) {
    // get the new byte:
    char inChar = (char)Serial1.read();
    // Serial.print(inChar);
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
