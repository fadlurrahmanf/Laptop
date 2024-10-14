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

const char ctrlByte = '10';
const uint8_t eeprom_size = 255;

#define DIGINP 14
#define DIGOUT 16

String payloadId() {
  String str = String(ctrlByte) + WIFI_SSID + "," + WIFI_PASS + "," + MQTT_ADDR + "," + MQTT_PORT + "," + MQTT_USER + "," + MQTT_PASS + "," + TOPIC_DATA + "," + TOPIC_STATUS + "," + TOPIC_COMMAND + "&&";
  return str;
}

void save_string_to_eeprom(String stringIn) {
  char buf[eeprom_size];
  stringIn.toCharArray(buf, stringIn.length());
  for (int i = 0; i < stringIn.length(); i++) {
    EEPROM.write(i, buf[i]);
  }
  EEPROM.commit();
}

String read_string_from_eeprom() {
  char bufferIn[eeprom_size];
  for (int i = 0; i < eeprom_size; i++) {
    bufferIn[i] = EEPROM.read(i);
  }
  String str = String(bufferIn);
  int pos = str.indexOf('&');
  str = str.substring(1, pos);
  return str;
}

int setWifiSSID(int argc, char **argv) {
  if (argc != 2) {
    shell.println("bad argument count");
    return -1;
  }
  WIFI_SSID = argv[1];
  shell.print("Setting WIFI_SSID to ");
  shell.println(WIFI_SSID);
  String str = payloadId();
  save_string_to_eeprom(str);
  return EXIT_SUCCESS;
}

int setWifiPASS(int argc, char **argv) {
  if (argc != 2) {
    shell.println("bad argument count");
    return -1;
  }
  WIFI_PASS = argv[1];
  shell.print("Setting WIFI_PASS to ");
  shell.println(WIFI_PASS);
  String str = payloadId();
  save_string_to_eeprom(str);
  return EXIT_SUCCESS;
}

int setMQTTADDR(int argc, char **argv) {
  if (argc != 2) {
    shell.println("bad argument count");
    return -1;
  }
  MQTT_ADDR = argv[1];
  shell.print("Setting MQTT_ADDR to ");
  shell.println(MQTT_ADDR);
  String str = payloadId();
  save_string_to_eeprom(str);
  return EXIT_SUCCESS;
}

int setMQTTPORT(int argc, char **argv) {
  if (argc != 2) {
    shell.println("bad argument count");
    return -1;
  }
  MQTT_PORT = argv[1];
  shell.print("Setting MQTT_PORT to ");
  shell.println(MQTT_PORT);
  String str = payloadId();
  save_string_to_eeprom(str);
  return EXIT_SUCCESS;
}

int setMQTTUSER(int argc, char **argv) {
  if (argc != 2) {
    shell.println("bad argument count");
    return -1;
  }
  MQTT_USER = argv[1];
  shell.print("Setting MQTT_USER to ");
  shell.println(MQTT_USER);
  String str = payloadId();
  save_string_to_eeprom(str);
  return EXIT_SUCCESS;
}

int setMQTTPASS(int argc, char **argv) {
  if (argc != 2) {
    shell.println("bad argument count");
    return -1;
  }
  MQTT_PASS = argv[1];
  shell.print("Setting MQTT_PASS to ");
  shell.println(MQTT_PASS);
  String str = payloadId();
  save_string_to_eeprom(str);
  return EXIT_SUCCESS;
}

int setMQTTTOPICDATA(int argc, char **argv) {
  if (argc != 2) {
    shell.println("bad argument count");
    return -1;
  }
  TOPIC_DATA = argv[1];
  shell.print("Setting TOPIC_DATA to ");
  shell.println(TOPIC_DATA);
  String str = payloadId();
  save_string_to_eeprom(str);
  return EXIT_SUCCESS;
}

int setMQTTTOPICSTATUS(int argc, char **argv) {
  if (argc != 2) {
    shell.println("bad argument count");
    return -1;
  }
  TOPIC_STATUS = argv[1];
  shell.print("Setting TOPIC_STATUS to ");
  shell.println(TOPIC_STATUS);
  String str = payloadId();
  save_string_to_eeprom(str);
  return EXIT_SUCCESS;
}

int setMQTTTOPICCOMMAND(int argc, char **argv) {
  if (argc != 2) {
    shell.println("bad argument count");
    return -1;
  }
  TOPIC_COMMAND = argv[1];
  shell.print("Setting TOPIC_COMMAND to ");
  shell.println(TOPIC_COMMAND);
  String str = payloadId();
  save_string_to_eeprom(str);
  return EXIT_SUCCESS;
}

int getInfo(int /*argc*/, char ** /*argv*/) {
  shell.print("Wifi SSID is ");
  shell.println(WIFI_SSID);
  shell.print("Wifi PASS is ");
  shell.println(WIFI_PASS);
  shell.print("MQTT ADDR is ");
  shell.println(MQTT_ADDR);
  shell.print("MQTT PORT is ");
  shell.println(MQTT_PORT);
  shell.print("MQTT USER is ");
  shell.println(MQTT_USER);
  shell.print("MQTT PASS is ");
  shell.println(MQTT_PASS);
  shell.print("MQTT TOPIC DATA is ");
  shell.println(TOPIC_DATA);
  shell.print("MQTT TOPIC STATUS is ");
  shell.println(TOPIC_STATUS);
  shell.print("MQTT TOPIC COMMAND is ");
  shell.println(TOPIC_COMMAND);
  return EXIT_SUCCESS;
}

int nodeRestart(int /*argc*/, char ** /*argv*/) {
  shell.println("node will be restarted");
  delay(1000);
  ESP.restart();
  return 0;
}

int showID(int /*argc*/ = 0, char ** /*argv*/ = NULL) {
  shell.println(F("Running " __FILE__ ", Built " __DATE__));
  return 0;
};

String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void setup() {

  EEPROM.begin(eeprom_size);
  char ctrlBt = EEPROM.read(0);
  if (ctrlByte == ctrlBt) {
    String str = read_string_from_eeprom();
    // String str = "@"+WIFI_SSID+","+WIFI_PASS+","+MQTT_ADDR+","+MQTT_PORT+","+MQTT_USER+","+MQTT_PASS+","+TOPIC_DATA+","+TOPIC_STATUS+","+TOPIC_COMMAND+"&&";
    WIFI_SSID = getValue(str, ',', 0);
    WIFI_PASS = getValue(str, ',', 1);
    MQTT_ADDR = getValue(str, ',', 2);
    MQTT_PORT = getValue(str, ',', 3);
    MQTT_USER = getValue(str, ',', 4);
    MQTT_PASS = getValue(str, ',', 5);
    TOPIC_DATA = getValue(str, ',', 6);
    TOPIC_STATUS = getValue(str, ',', 7);
    TOPIC_COMMAND = getValue(str, ',', 8);
  } else {
    String str = payloadId();
    save_string_to_eeprom(str);
  }

  pinMode(DIGINP, INPUT_PULLUP);
 
  delay(10);
  if (digitalRead(DIGINP)) {
    beginFDRS();
  } else {
    Serial.begin(115200);
    shell.attach(Serial);
    shell.addCommand(F("id?"), showID);
    shell.addCommand(F("setWifiSSID <WIFI_SSID>"), setWifiSSID);
    shell.addCommand(F("setWifiPASS <WIFI_PASS>"), setWifiPASS);
    shell.addCommand(F("setMQTTADDR <MQTT_ADDR>"), setMQTTADDR);
    shell.addCommand(F("setMQTTPORT <MQTT_PORT>"), setMQTTPORT);
    shell.addCommand(F("setMQTTUSER <MQTT_USER>"), setMQTTUSER);
    shell.addCommand(F("setMQTTPASS <MQTT_PASS>"), setMQTTPASS);
    shell.addCommand(F("setMQTTTOPICDATA <TOPIC_DATA>"), setMQTTTOPICDATA);
    shell.addCommand(F("setMQTTTOPICSTATUS <TOPIC_STATUS>"), setMQTTTOPICSTATUS);
    shell.addCommand(F("setMQTTTOPICCOMMAND <TOPIC_COMMAND>"), setMQTTTOPICCOMMAND);
    shell.addCommand(F("getInfo"), getInfo);
    shell.addCommand(F("nodeRestart"), nodeRestart);
    delay(3000);
    showID();
    payloadId();
  }
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,HIGH);
}

void loop() {
  digitalWrite(LED_BUILTIN,LOW);
  yield();
  if (digitalRead(DIGINP)) {
     yield();
    loopFDRS();
  } else {
    shell.executeIfInput();
  }
}
