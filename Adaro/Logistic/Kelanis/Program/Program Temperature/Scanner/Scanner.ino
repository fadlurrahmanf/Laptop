#include <SoftWire.h>
#include "I2CScanner.h"
#define SDA_PIN PB6
#define SCL_PIN PB7
#define VCCINA PB9
#define GNDINA PB8

I2CScanner scanner;
// SoftWire myWire(SDA_PIN, SCL_PIN);
SoftWire myWire(PB6, PB7);

void setup() {
  myWire.begin();
  Serial.begin(115200);
  while (!Serial) {};
  pinMode(VCCINA, OUTPUT);
  pinMode(GNDINA, OUTPUT);
  digitalWrite(VCCINA, HIGH);
  digitalWrite(GNDINA, LOW);

  scanner.Init();
  Serial.println("Scanner");
}

void loop() {
  scanner.Scan();
  delay(5000);
}
