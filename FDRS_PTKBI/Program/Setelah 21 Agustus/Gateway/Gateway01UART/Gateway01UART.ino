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

struct idStruc {
  uint8_t configId;
  uint8_t UartGtwyId;
  uint8_t EspNg1Id;
  uint8_t EspNg2Id;
  uint8_t LoraNg1Id;
  uint8_t LoraNg2Id;
} id;

bool needRst = false;

int showID(int /*argc*/ = 0, char ** /*argv*/ = NULL) {
  shell.println(F("Running " __FILE__ ", Built " __DATE__));
  return 0;
};

int setUartGatewayId(int argc, char **argv) {
  if (argc != 2) {
    shell.println("bad argument count");
    return -1;
  }
  id.UartGtwyId = atoi(argv[1]);
  shell.print("Setting UartGtwyId to ");
  shell.println(id.UartGtwyId);
  EEPROM.put(0, id);
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
  needRst = true;
  return EXIT_SUCCESS;
}

int setLoraNg1Id(int argc, char **argv) {
  if (argc != 2) {
    shell.println("bad argument count");
    return -1;
  }
  uint8_t a = atoi(argv[1]);
  id.LoraNg1Id = a;
  shell.print("Setting LoraNg1Id to ");
  shell.println(id.LoraNg1Id);
  EEPROM.put(0, id);
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
  needRst = true;
  return EXIT_SUCCESS;
}

int getInfo(int /*argc*/, char ** /*argv*/) {
  shell.print("UartGtwyId is ");
  shell.println(id.UartGtwyId);
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

  EEPROM.get(0, id);
  if (id.configId == 0x43) {
    UNIT_MAC = id.UartGtwyId;
    ESPNOW_NEIGHBOR_1 = id.EspNg1Id;
    ESPNOW_NEIGHBOR_2 = id.EspNg2Id;
    LORA_NEIGHBOR_1 = id.LoraNg1Id;
    LORA_NEIGHBOR_2 = id.LoraNg2Id;
  } else {
    id.configId = 0x43;
    id.UartGtwyId = UNIT_MAC;
    id.EspNg1Id = ESPNOW_NEIGHBOR_1;
    id.EspNg2Id = ESPNOW_NEIGHBOR_2;
    id.LoraNg1Id = LORA_NEIGHBOR_1;
    id.LoraNg2Id = LORA_NEIGHBOR_2;
    EEPROM.put(0, id);
  }
  beginFDRS();

  shell.attach(Serial);
  shell.addCommand(F("id?"), showID);
  shell.addCommand(F("setUartGatewayId <UNIT_MAC>"), setUartGatewayId);
  shell.addCommand(F("setEspNg1Id <ESPNOW_NEIGHBOR_1>"), setEspNg1Id);
  shell.addCommand(F("setEspNg2Id <ESPNOW_NEIGHBOR_2>"), setEspNg2Id);
  shell.addCommand(F("setLoraNg1Id <LORA_NEIGHBOR_1>"), setLoraNg1Id);
  shell.addCommand(F("setLoraNg2Id <LORA_NEIGHBOR_2>"), setLoraNg2Id);
  shell.addCommand(F("getInfo"), getInfo);
  delay(4000);
  showID();
  Serial.println("Start");
}

void loop() {
  loopFDRS();

  if (millis() - lastTimeOut > timeOut) {
    needRst = true;
  }

  if (needRst) {
    ESP.restart();
  }
  
  shell.executeIfInput();
}
