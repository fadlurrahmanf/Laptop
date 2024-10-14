//  FARM DATA RELAY SYSTEM
//
//  BME280 SENSOR MODULE
//
//  Developed by Timm Bogner (timmbogner@gmail.com) in Urbana, Illinois, USA.

#include "fdrs_node_config.h"
#include <Adafruit_BME280.h>
#include <fdrs_node.h>
#include <SimpleSerialShell.h>
#include <ESP_EEPROM.h>

#define DONE 14
#define CONFIG 0x01
#define LED LED_BUILTIN

bool needRst = false;
unsigned long previousMillis = 0;
const long interval = 1000;

struct idStruct {
  uint8_t configId;
  uint16_t nodeId;
  uint8_t gtwyId;
} id;
void blinkLed(int n) {
  for (int i = 0; i < n; i++) {
    digitalWrite(LED, LOW);
    delay(1);
    digitalWrite(LED, HIGH);
    delay(1);
  }
}
int setGatewayId(int argc, char **argv) {
  if (argc != 2) {
    shell.println("bad argument count");
    return -1;
  }
  id.gtwyId = atoi(argv[1]);
  shell.print("Setting networkId to ");
  shell.println(id.gtwyId);
  EEPROM.put(0, id);
  EEPROM.commit();
  needRst = true;
  return EXIT_SUCCESS;
}
int setNodeId(int argc, char **argv) {
  if (argc != 2) {
    shell.println("bad argument count");
    return -1;
  }
  id.nodeId = atoi(argv[1]);
  shell.print("Setting nodeId to ");
  shell.println(id.nodeId);
  EEPROM.put(0, id);
  EEPROM.commit();
  needRst = true;
  return EXIT_SUCCESS;
}
int getInfo(int /*argc*/ = 0, char ** /*argv*/ = NULL) {
  shell.println(F("Running " __FILE__ ", Built " __DATE__));
  shell.print("NodeId is ");
  shell.println(id.nodeId);
  shell.print("gtwyId is ");
  shell.println(id.gtwyId);
  return EXIT_SUCCESS;
}

Adafruit_BME280 bme;
ADC_MODE(ADC_VCC);

void setup() {
  pinMode(DONE, OUTPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(DONE, LOW);
  digitalWrite(LED,HIGH);

  EEPROM.begin(sizeof(idStruct));
  EEPROM.get(0, id);
  if (id.configId == CONFIG) {
    READING_ID = id.nodeId;
    GTWY_MAC = id.gtwyId;
  } else {
    id.configId = CONFIG;
    id.nodeId = READING_ID;
    id.gtwyId = GTWY_MAC;
    bool ok = EEPROM.commit();
  }
  // Serial.begin(115200);
  beginFDRS();
  while (!bme.begin(0x76)) {
    //Serial.println("BME not initializing!");
    delay(10);
  }

  shell.attach(Serial);
  shell.addCommand(F("setGatewayId <gtwyId>"), setGatewayId);
  shell.addCommand(F("setNodeId <nodeId>"), setNodeId);
  shell.addCommand(F("getInfo"), getInfo);
  Serial.println();
  delay(2000);
  getInfo();
}

void loop() {
  if (needRst) {
    ESP.restart();
  }
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    loadFDRS(bme.readTemperature(), TEMP_T);
    loadFDRS(bme.readHumidity(), HUMIDITY_T);
    // loadFDRS(bme.readPressure() / 100.0F, PRESSURE_T);
    loadFDRS(ESP.getVcc(), VOLTAGE_T);
    if (sendFDRS()) {
      blinkLed(1);
      digitalWrite(DONE, HIGH);
      delay(100);
      digitalWrite(DONE, LOW);
    }
  }
  shell.executeIfInput();
  // sleepFDRS(10);  //Sleep time in seconds
}
