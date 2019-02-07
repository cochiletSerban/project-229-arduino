#include <RGBLed.h>

#include <Wire.h>

bool goClub = false;    // true if club mode is enabled
bool goLobby = false;

// holds values for the roof rgb strip
struct Roof {
    int red;
    int green;
    int blue;
    int brightness;
    int anim;
};
struct Roof roof;


int mode = 0; // determins the mode
int prevMode = 0;
int wg = 0;   // holds values for wall stirps
int wb = 0;

const int wgPin = 3;    // wall stips pins
const int wbPin = 6;

int analogPin = 0;  // pin for reading msgeq7 data
int strobePin = 5;  // strobe pin for msgeq7
int resetPin = 4;   // reset pin for msgeq7

RGBLed roofLed(9, 10, 11, COMMON_CATHODE);  // pins for rgb roof strip

int brightness = 0;    // how bright the LED is
int fadeAmount = 1;  

const int maxlength = 8;    // max len of ic2 recived data
unsigned char buffer[maxlength];    // i2c recive buffer
unsigned char printable[maxlength]; // used to copy the data form ^
int received = 0;   // used to determine how many bytes were received

int spectrumValue[7];   // buffer for msgeq7 data
int filter = 110;   // msgeq7 noise filter

void setup() {
    Wire.begin(8); // join i2c bus with address #8
    Wire.onReceive(receiveEvent); // register event

    Serial.begin(9600); // start serial for output

    pinMode(wgPin, OUTPUT);     // defines pin wgPin as Output for wgStrip
    pinMode(wbPin, OUTPUT);     // defines pin wbPin as Output for wbStrip
    pinMode(analogPin, INPUT);  // defines analog pin A0 as an Input for msgeq7
    pinMode(strobePin, OUTPUT); // defines strobe pin 2 as Output for msgeq7
    pinMode(resetPin, OUTPUT);  // defines reset pin 3 as Output for msgeq7
}

// prints state values used for debuging
void printState() {
    Serial.print(mode);
    Serial.print(" ");
    Serial.print(roof.red);
    Serial.print(" ");
    Serial.print(roof.green);
    Serial.print(" ");
    Serial.print(roof.blue);
    Serial.print(" ");
    Serial.print(roof.brightness);
    Serial.print(" ");
    Serial.print(roof.anim);
    Serial.print(" ");
    Serial.print(wg);
    Serial.print(" ");
    Serial.print(wb);
    Serial.println("");
}

// copies data form buffer to variabiles and stuctures
void setStateToVars() {
    memcpy(printable, buffer, maxlength);
    prevMode = mode;
    mode = printable[0];
    roof.red = printable[1];
    roof.green = printable[2];
    roof.blue = printable[3];
    roof.brightness = printable[4];
    roof.anim = printable[5];
    wg = printable[6];
    wb = printable[7];
}

void lobbyAnimation() {

  roofLed.setColor(0, 50, 0);
  delay(800);
  analogWrite(wbPin, 50);
  delay(800);
  analogWrite(wgPin, 50);
  delay(800);
  
  analogWrite(wgPin, 0);
  delay(800);
  analogWrite(wbPin, 0);
  delay(800);
  roofLed.off();
  delay(800);
  

}

// determins the mode and cleans the curent mode on mode switch
void setMode() {
    if (mode == 2) {
        cleanState();
        goClub = true;
    }
    if (mode != 2) {
        goClub = false;
        //cleanState();
    }
    if (mode == 1) {
        if (prevMode == 2) cleanState();
        writeToStrips();
    }
    if (mode == 0) {
        cleanState();
    }
    if (mode == 3) {
      cleanState();
      goLobby = true;
    }
    if (mode != 3) {
      goLobby = false;
    }
}

// resets curent state
void cleanState() {
    roofLed.brightness(255, 255, 255, 0);
    analogWrite(wgPin, 0);
    analogWrite(wbPin, 0);
}

void loop() {

    if (received > 0) {
        setStateToVars();
        printState(); // debug
        setMode();
        received = 0;
    }

    // club mode, read form msgeq filter map and write to leds
    if (goClub) {
        digitalWrite(resetPin, HIGH);
        digitalWrite(resetPin, LOW);

        for (int i = 0; i < 7; i++) {
            digitalWrite(strobePin, LOW);

            spectrumValue[i] = analogRead(analogPin);

            //  noise filtering
            if (spectrumValue[i] < filter) spectrumValue[i] = 0;

            // misc tweaks
            if (i > 2 && i < 5) spectrumValue[i] = max(spectrumValue[i] - 10, 0);
            if (i == 5) spectrumValue[i] = max(spectrumValue[5] - 100, 0) * 1.2;
            if (i == 6) spectrumValue[i] = max(spectrumValue[6] - 120, 0) * 2;

           

            spectrumValue[i] = map(spectrumValue[i], 0, 1023, 0, 255);
           // Serial.print(spectrumValue[i]);
           // Serial.print(" ");
            digitalWrite(strobePin, HIGH);
        }
           // Serial.println();
        roofLed.setColor(spectrumValue[1], spectrumValue[0], (spectrumValue[1] + spectrumValue[0]) / 2); // bass
        analogWrite(wbPin, spectrumValue[4]); //med/voice
        analogWrite(wbPin, spectrumValue[2]); //med/voice
        analogWrite(wbPin, spectrumValue[3]); //med/voice
        analogWrite(wgPin, spectrumValue[5]); //hh & symbols
        analogWrite(wgPin, spectrumValue[6]); //hh & symbols
    }

    if (goLobby) {
      lobbyAnimation();
    }

}

// sets values to led strips
void writeToStrips() {
    Serial.println("writing to strips"); // debug
    printState();
    analogWrite(wgPin, wg);
    analogWrite(wbPin, wb);
    roofLed.brightness(roof.red, roof.green, roof.blue, roof.brightness);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
    received = howMany;
    memset(buffer, 0, maxlength);

    for (int i = 0; i < howMany; i++) {
        buffer[i] = Wire.read();
    }
}
