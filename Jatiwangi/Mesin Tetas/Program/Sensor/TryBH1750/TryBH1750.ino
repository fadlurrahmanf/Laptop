//  FARM DATA RELAY SYSTEM
//
//  BME280 SENSOR MODULE
//
//  Developed by Timm Bogner (timmbogner@gmail.com) in Urbana, Illinois, USA.

#include "fdrs_node_config.h"
// #include <BH1750.h>
// #include <Wire.h>
#include <fdrs_node.h>

// BH1750 lightMeter;

void setup() {
  beginFDRS();
  // Wire.begin();
  // lightMeter.begin();
}

void loop() {
  // float lux = lightMeter.readLightLevel();
  float lux = 0;
  loadFDRS(lux, LIGHT_T);
  sendFDRS();
  delay (60000);
  // sleepFDRS(60);  //Sleep time in seconds
}
