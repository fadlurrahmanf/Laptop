//  FARM DATA RELAY SYSTEM
//
//  BME280 SENSOR MODULE
//
//  Developed by Timm Bogner (timmbogner@gmail.com) in Urbana, Illinois, USA.

#include "fdrs_node_config.h"
#include <Adafruit_BME280.h>
#include <fdrs_node.h>
// unsigned long previousMillis = 0;  // will store last time LED was updated
//const int updatePeriode = 10000;
//int waktuKirim = updatePeriode - random (5000);
// constants won't change:
// const long interval = 10000;  // interval at which to blink (milliseconds)

Adafruit_BME280 bme;

void setup() {
  //Serial.begin(115200);
  beginFDRS();
  while (!bme.begin(0x76)) {
    //Serial.println("BME not initializing!");
    delay(10);
  }
}

void loop() {
  //  loadFDRS(bme.readTemperature(), TEMP_T);
  //  loadFDRS(bme.readHumidity(), HUMIDITY_T);
  // loadFDRS(bme.readPressure() / 100.0F, PRESSURE_T);


  // unsigned long currentMillis = millis();
  //long interval = baseInterval - random(0, 100);  // Randomized interval between 4000-5000 ms

  // if (currentMillis - previousMillis >= interval) {
  // save the last time the event occurred
  // previousMillis = currentMillis - random(1000);
  loadFDRS(bme.readTemperature(), TEMP_T);
  loadFDRS(bme.readHumidity(), HUMIDITY_T);
  sendFDRS();
  sleepFDRS(60 - random (5));  //Sleep time in seconds
  // }
  // delay (waktuKirim);
}
