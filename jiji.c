#include <Wire.h>

struct Roof {
  int r;
  int g;
  int b;
  int anim;
};
struct Roof roof;
int mode;
int wg;
int wb;

void setup() {
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  delay(250);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  //while (Wire.available()) { // loop through all but the last
   mode = Wire.read();
   roof.r = Wire.read();
   roof.g = Wire.read();
   roof.b = Wire.read();
   roof.anim = Wire.read();
   wg = Wire.read();
   wb = Wire.read();
   Serial.println(mode);
       delay(250);
  //}
}
