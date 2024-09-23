#include <Q2HX711.h>

const byte hx711_data_pin = PA0;
const byte hx711_clock_pin = PA1;
const uint8_t numReadings = 10; // Adjust this based on your preference
uint8_t count = 0;
float readings[numReadings];
Q2HX711 hx711(hx711_data_pin, hx711_clock_pin);

void setup() {
  Serial.begin(115200);
}

void loop() {
  float sensorValue = hx711.read() / 100.0;
  readings[count] = sensorValue;
  count = (count + 1) % numReadings;
  float average = 0;
  for (int i = 0; i < numReadings; i++) {
    average += readings[i];
  }
  average /= numReadings;
  Serial.println(average);
  delay(10);
}
