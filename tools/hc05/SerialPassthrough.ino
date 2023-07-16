#include "SoftwareSerial.h"

SoftwareSerial SWSerial(10, 11);

void setup() {
  // AT Mode
  //Serial.begin(38400);
  //SWSerial.begin(38400);

  // Passthrough mode
  Serial.begin(115200);
  SWSerial.begin(115200);
}

void loop() {
  if (Serial.available()) {
    int b = Serial.read();
    Serial.write(b); // local echo
    if (b == '\n') {
      SWSerial.write('\r');
      SWSerial.write('\n');
    } else {
      SWSerial.write(b);
    }
  }

  if (SWSerial.available()) {       // If anything comes in Serial1 (pins 0 & 1)
    Serial.write(SWSerial.read());  // read it and send it out Serial (USB)
  }
}
