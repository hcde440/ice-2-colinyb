// Adafruit IO Digital Input Example
// Tutorial Link: https://learn.adafruit.com/adafruit-io-basics-digital-input
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#include "config.h"

/************************ Example Starts Here *******************************/

// Adafruit dashboard: https://io.adafruit.com/crysis/dashboards/ice-2

// initializing pins for the two buttons and the LDR
#define BUTTON_PIN 2
#define BUTTON2_PIN 0
#define LDR_PIN A0

// intial states for buttons and for keeping track of state changes
bool current1 = false;
bool last1 = false;
bool current2 = false;
bool last2 = false;
bool change = false;
bool change2 = false;

int startTime = millis();

// set up for the various adafruit io feeds
AdafruitIO_Feed *button = io.feed("button");
AdafruitIO_Feed *button2 = io.feed("button2");
AdafruitIO_Feed *light = io.feed("ldr");

void setup() {

  // set button pin as an input with pullup resistor
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  pinMode(LDR_PIN, INPUT); //set the LDR as an input

  // start the serial connection
  Serial.begin(115200);
  Serial.print("This board is running: ");
  Serial.println(F(__FILE__));
  Serial.print("Compiled: ");
  Serial.println(F(__DATE__ " " __TIME__));
   
  // wait for serial monitor to open
  while(! Serial);

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

}

void loop() {
  
  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

  //timer for making sure the LDR sensor doesn't constantly get updated (around 15 times a second).
  //instead with this timer it will only send every 10 seconds
  if (startTime + 10000 <= millis()) { //if the time elapsed since the last sent data was 10 or more seconds...
    light->save(analogRead(LDR_PIN)); //send the data to adafruit io
    Serial.println("SENT LIGHT READING"); //print to serial
    startTime = millis(); //re-set the start time of the timer
  }

  // grab the current state of the button.
  // we have to flip the logic because we are
  // using a pullup resistor.
  if(digitalRead(BUTTON_PIN) == LOW){
    current1 = true;
  }else{
    current1 = false;
  }

  if(digitalRead(BUTTON2_PIN) == LOW){ //check button state and if it is low, set current to true otherwise set it to false
    current2 = true;
  }else{
    current2 = false;
  }

  // return if the value hasn't changed
  if(current1 == last1) { //if the state of the first button stayed the same, update the change boolean to true
    change = true;
  }

  if(current2 == last2) { //if the state of the second button stayed the same, update the change boolean to true
    change2 = true;
  }

  if(change && change2) {
    change = false; //return buttons to false for next cycle of the loop function
    change2 = false;
    return;
  }

  // save the current state to the 'button' or 'button2' feed on adafruit io
  Serial.print("sending buttons -> ");
  if(!change) { // if the first button has a state change
    Serial.println("Button 1: " + current1);
    button->save(current1);
  }
  if(!change2) { // if the second button has a state change
    Serial.println("Button2: " + current2);
    button2->save(current2);
  }
  

  // store last button state
  last1 = current1;
  last2 = current2;

}
