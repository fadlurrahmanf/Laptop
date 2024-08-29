//  FARM DATA RELAY SYSTEM
//
//  GATEWAY 2.000
//
//  Developed by Timm Bogner (timmbogner@gmail.com) in Urbana, Illinois, USA.
//

#include "fdrs_gateway_config.h"
#include <fdrs_gateway.h>
#include <SimpleSerialShell.h>
#include <EEPROM.h>

#define LED  2

struct idStruc {
  uint8_t configId;
  uint8_t LoRaRepeaterId;
  uint8_t EspNg1Id;
  uint8_t EspNg2Id;
  uint8_t LoraNg1Id;
  uint8_t LoraNg2Id;
} id;

void blinkLed(int n) {
  for (int i=0;i<n;i++) {
    digitalWrite(LED,LOW);
    delay(200);
    digitalWrite(LED, HIGH);
    delay(200);
  }
}

bool needRst = false;

int showID(int /*argc*/ = 0, char ** /*argv*/ = NULL) {
  shell.println(F("Running " __FILE__ ", Built " __DATE__));
  return 0;
};

int setLoRaRepeaterId(int argc, char **argv) {
  if (argc != 2) {
    shell.println("bad argument count");
    return -1;
  }
  id.LoRaRepeaterId = atoi(argv[1]);
  shell.print("Setting LoRaRepeaterId to ");
  shell.println(id.LoRaRepeaterId);
  EEPROM.put(0, id);
  EEPROM.commit();
  needRst = true;
  return EXIT_SUCCESS;
}

int setEspNg1Id(int argc, char **argv) {
  if (argc != 2) {
    shell.println("bad argument count");
    return -1;
  }
  id.EspNg1Id = atoi(argv[1]);
  shell.print("Setting EspNg1Id to ");
  shell.println(id.EspNg1Id);
  EEPROM.put(0, id);
  EEPROM.commit();
  needRst = true;
  return EXIT_SUCCESS;
}

int setEspNg2Id(int argc, char **argv) {
  if (argc != 2) {
    shell.println("bad argument count");
    return -1;
  }
  id.EspNg2Id = atoi(argv[1]);
  shell.print("Setting EspNg2Id to ");
  shell.println(id.EspNg2Id);
  EEPROM.put(0, id);
  EEPROM.commit();
  needRst = true;
  return EXIT_SUCCESS;
}

int setLoraNg1Id(int argc, char **argv) {
  if (argc != 2) {
    shell.println("bad argument count");
    return -1;
  }
  id.LoraNg1Id = atoi(argv[1]);
  shell.print("Setting LoraNg1Id to ");
  shell.println(id.LoraNg1Id);
  EEPROM.put(0, id);
  EEPROM.commit();
  needRst = true;
  return EXIT_SUCCESS;
}

int setLoraNg2Id(int argc, char **argv) {
  if (argc != 2) {
    shell.println("bad argument count");
    return -1;
  }
  id.LoraNg2Id = atoi(argv[1]);
  shell.print("Setting LoraNg1Id to ");
  shell.println(id.LoraNg2Id);
  EEPROM.put(0, id);
  EEPROM.commit();
  needRst = true;
  return EXIT_SUCCESS;
}

int getInfo(int /*argc*/, char ** /*argv*/) {
  shell.print("LoRaRepeaterId is ");
  shell.println(id.LoRaRepeaterId);
  shell.print("ESPNOW_NEIGHBOR_1  is ");
  shell.println(id.EspNg1Id);
  shell.print("ESPNOW_NEIGHBOR_2 is ");
  shell.println(id.EspNg2Id);
  shell.print("LORA_NEIGHBOR_1 is ");
  shell.println(id.LoraNg1Id);
  shell.print("LORA_NEIGHBOR_2 is ");
  shell.println(id.LoraNg2Id);
  return EXIT_SUCCESS;
}

int nodeRestart(int /*argc*/, char ** /*argv*/) {
  shell.println("node will be restarted");
  delay(1000);
  ESP.restart();
  return 0;
}

void setup() {
  pinMode(LED, OUTPUT);
  EEPROM.begin(sizeof(idStruc));
  EEPROM.get(0, id);
  if (id.configId == 0x43) {
    UNIT_MAC = id.LoRaRepeaterId;
    ESPNOW_NEIGHBOR_1 = id.EspNg1Id;
    ESPNOW_NEIGHBOR_2 = id.EspNg2Id;
    LORA_NEIGHBOR_1 = id.LoraNg1Id;
    LORA_NEIGHBOR_2 = id.LoraNg2Id;
  } else {
    id.configId = 0x43;
    id.LoRaRepeaterId = UNIT_MAC;
    id.EspNg1Id = ESPNOW_NEIGHBOR_1;
    id.EspNg2Id = ESPNOW_NEIGHBOR_2;
    id.LoraNg1Id = LORA_NEIGHBOR_1;
    id.LoraNg2Id = LORA_NEIGHBOR_2;
    EEPROM.put(0, id);
    bool ok = EEPROM.commit();
  }
  
  beginFDRS();

  shell.attach(Serial);
  shell.addCommand(F("id?"), showID);
  shell.addCommand(F("setUartGatewayId <UNIT_MAC>"), setLoRaRepeaterId);
  shell.addCommand(F("setEspNg1Id <ESPNOW_NEIGHBOR_1>"), setEspNg1Id);
  shell.addCommand(F("setEspNg2Id <ESPNOW_NEIGHBOR_2>"), setEspNg2Id);
  shell.addCommand(F("setLoraNg1Id <LORA_NEIGHBOR_1>"), setLoraNg1Id);
  shell.addCommand(F("setLoraNg2Id <LORA_NEIGHBOR_2>"), setLoraNg2Id);
  shell.addCommand(F("getInfo"), getInfo);
  showID();

  blinkLed(3);
}

void loop() {
  loopFDRS();
  if (needRst) {
    ESP.restart();
  }  
  shell.executeIfInput();
}
