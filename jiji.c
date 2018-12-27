// Wire Slave Receiver
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Receives data as an I2C/TWI slave device
// Refer to the "Wire Master Writer" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>

bool goClub = false;

struct Roof {
  int red;
  int green;
  int blue;
  int brightness;
  int anim;
};

struct Roof roof;

   
int mode = 0;
int wg = 0;
int wb = 0;

const int wgPin = 9;
const int wbPin = 10;

const int maxlength = 8;
//chage to rgb lib
const int roofRedPin = 13;
const int roofGreenPin = 14;
const int roofBluePin = 15;
unsigned char buffer[maxlength];
unsigned char printable[maxlength];
int received = 0;

void setup() {
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output
  pinMode(wgPin, OUTPUT);
  pinMode(wbPin, OUTPUT);
  pinMode(roofRedPin, OUTPUT);
  pinMode(roofGreenPin, OUTPUT);
  pinMode(roofBluePin, OUTPUT);
}

void printState(){
    Serial.print(mode);
    Serial.print(F(" "));
    Serial.print(roof.red);
    Serial.print(F(" "));
    Serial.print(roof.green);
    Serial.print(F(" "));
    Serial.print(roof.blue);
    Serial.print(F(" "));
    Serial.print(roof.brightness);
    Serial.print(F(" "));
    Serial.print(roof.anim);
    Serial.print(F(" "));
    Serial.print(wg);
    Serial.print(F(" "));
    Serial.print(wb);
    Serial.println(F(" "));
}

void setStateToVars() {
    memcpy(printable, buffer, maxlength);
    mode = printable[0];
    roof.red = printable[1];
    roof.green = printable[2];
    roof.blue = printable[3];
    roof.brightness = printable[4];
    roof.anim = printable[5];
    wg = printable[6];
    wb = printable[7];
}

void setMode() {
    if (mode == 2) goClub = true;
    if (mode != 2) goClub = false;
    if (mode == 1) {
      writeToStrips();
    }
}

void loop() {
  
  if (received) {
      setStateToVars();
      //printState();
      setMode();
      received = 0;
  }

  if (goClub) Serial.print(F("party "));

  
}

void writeToStrips() {
  Serial.println(F("writing to strips"));
  printState();
  analogWrite(wgPin, wg);
  analogWrite(wbPin, wb);
  /// change to rgb lib
  analogWrite(roofRedPin , roof.red);
  analogWrite(roofGreenPin, roof.green) ; 
  analogWrite(roofBluePin, roof.blue);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  received = howMany;
  memset(buffer, 0, maxlength);

  for (int i = 0; i < howMany;buffer[i] = Wire.read());
}
