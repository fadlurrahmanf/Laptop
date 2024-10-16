#include <RadioLib.h>
#include <IWatchdog.h>
#include <EEPROM.h>
#include "PCF8574.h"

#define LED        PC13
#define LR_RST     PC4
#define NSS        PA4
#define DIO0       PB5
#define DIO1       PB4
#define DIO2       PB3
#define LORA_ON    PB8   // active LOW
#define ADDI2C1    0x39
#define ADDI2C2    0x3B
#define DIGINP     PA0

SX1276 lora = new Module(NSS, DIO0, LR_RST, DIO1);

String DEVID = "RLY100";
uint32_t updateRate = 120000;
long lastUpdate;
bool transmitDone = false;
uint16_t statusRelay = 0;
bool modeSta = true;

PCF8574 PCF1(ADDI2C1);
PCF8574 PCF2(ADDI2C2);

//eeprom address
const uint32_t config_addr = 0;
const uint32_t devid_addr = 1;
const uint32_t mode_addr = 2;
const uint32_t periode_add = 3;

String xorChecksum(String s) {
  byte b = s.charAt(0);
  for (int i = 1; i < s.length(); i++) {
    b = b ^ s.charAt(i);
  }
  String checksum = String(b, HEX);
  if (checksum.length() == 1) checksum = "0" + checksum;
  return checksum;
}

void blinkLed(int n) {
  for (int i = 0; i < n; i++) {
    digitalWrite(LED, LOW);
    delay(200);
    digitalWrite(LED, HIGH);
    delay(200);
  }
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
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

// flag to indicate that a packet was received
volatile bool receivedFlag = false;

// disable interrupt when it's not needed
volatile bool enableInterrupt = true;

// this function is called when a complete packet
// is received by the module
// IMPORTANT: this function MUST be 'void' type
//            and MUST NOT have any arguments!
void setFlag(void) {
  // check if the interrupt is enabled
  if (!enableInterrupt) {
    return;
  }

  // we got a packet, set the flag
  receivedFlag = true;
}

void Relay(byte n, bool b) {
  n = n - 1;
  if (n < 8) {
    if (b) {
      PCF1.write(n, LOW);
    } else {
      PCF1.write(n, HIGH);
    }
  } else {
    n = n - 8;
    if (b) {
      PCF2.write(n, LOW);
    } else {
      PCF2.write(n, HIGH);
    }
  }
}

void sendDataLora(String str) {

  String str1 = xorChecksum(str);

  str += ":" + str1 + "$";

  int state = lora.scanChannel();
  int cacah = 0;
  // tunggu sampai channel nya free
  while ((state == PREAMBLE_DETECTED) && (cacah < 5)) {
    delay(random(100, 1000));
    state = lora.scanChannel();
    cacah++;
  }
  state = lora.transmit(str.c_str());
  //Serial.println(str);
  if (state == ERR_NONE) {
    // the packet was successfully transmitted
    blinkLed(2);
  }

  //transmitDone = false;
  state = lora.startReceive();

}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  PCF1.begin();
  PCF2.begin();

  pinMode(LED, OUTPUT);
  pinMode(LORA_ON, OUTPUT);
  digitalWrite(LORA_ON, LOW);
  pinMode(DIGINP, INPUT);

  if (IWatchdog.isReset(true)) {
    // LED blinks to indicate reset
    blinkLed(10);
  }

  uint8_t config_code = EEPROM.read(config_addr);
  if (config_code == 40) {
    uint8_t devid_code = EEPROM.read(devid_addr);
    uint8_t mode_code = EEPROM.read(mode_addr);
    uint8_t periode_code = EEPROM.read(periode_add);
    DEVID = "RLY" + String(devid_code);
    if (mode_code == 0) {     // manual
      modeSta = false;
    } else {
      modeSta = true;
    }
    updateRate = periode_code * 1000;
  } else {
    EEPROM.write(config_addr, 40);  // config code
    EEPROM.write(devid_addr, 100);  // default devid address
    EEPROM.write(mode_addr, 1);   // manual transmit
    EEPROM.write(periode_add, 120);
  }

  int state = lora.begin(915.0, 125.0, 9, 7, SX127X_SYNC_WORD, 17, 8, 0);
  if (state == ERR_NONE) {
    //Serial.println(F("success!"));
  } else {
    //Serial.print(F("failed, code "));
    //Serial.println(state);
    blinkLed(20);
    while (true);
  }

  lastUpdate = millis();

  // set the function that will be called
  // when new packet is received
  lora.setDio0Action(setFlag);

  state = lora.startReceive();

  //analogReadResolution(12);

  // Init the watchdog timer with 10 seconds timeout
  IWatchdog.begin(10000000);
  lastUpdate = millis();
  transmitDone = false;

  blinkLed(3);

}

void loop() {
  // put your main code here, to run repeatedly:

  IWatchdog.reload();

  if (!modeSta) {
    long now = millis();
    if ((now - lastUpdate) > updateRate) {
      lastUpdate = now;
      String msg = String(DEVID) + ",ST," + String(statusRelay, HEX) + "," + String(digitalRead(DIGINP));
      sendDataLora(msg);
    }
  }

  if (receivedFlag) {
    // disable the interrupt service routine while
    // processing the data
    enableInterrupt = false;

    // reset flag
    receivedFlag = false;

    if (transmitDone) {
      transmitDone = false;
    } else {
      // you can read received data as an Arduino String
      String str = "";
      int state = lora.readData(str);
      bool dataValid = false;  // data from the server
      int pos = str.indexOf("#");
      if (pos > 4) {
        str = str.substring(0, pos);
        String payload = getValue(str, ':', 0);
        String checkSum = getValue(str, ':', 1);
        String calcChecksum = xorChecksum(payload);
        if (checkSum == calcChecksum) {
          dataValid = true;
          str = payload;
        }
      }

      if (state == ERR_NONE) {
        if (dataValid) {
          // packet was successfully received
          bool rlySta = false;
          String devid = getValue(str, ',', 0);  // DEVID, CMD, RLYNum, 0/1

          if (devid == DEVID) {
            String cmd = getValue(str, ',', 1);
            if (cmd == "0") {
              String msg = String(DEVID) + ",ST," + String(statusRelay, HEX) + "," + String(digitalRead(DIGINP));
              delay(1000);
              sendDataLora(msg);
            }
            if (cmd == "1") {       //devid
              String cfg = getValue(str, ',', 2);
              DEVID = "RLY" + cfg;
              EEPROM.write(devid_addr, cfg.toInt());
            }
            if (cmd == "2") {      //mode
              String cfg = getValue(str, ',', 2);
              if (cfg == "0") {
                modeSta = false;
              } else {
                modeSta = true;
              }
              EEPROM.write(mode_addr, cfg.toInt());
            }
            if (cmd == "3") {      //mode
              String cfg = getValue(str, ',', 2);
              uint8_t periode = cfg.toInt();
              updateRate = periode * 1000;
              EEPROM.write(periode_add, periode);
            }
            if (cmd == "4") {      //mode
              String rlyNum = getValue(str, ',', 2);
              int relayNum = rlyNum.toInt();
              if ((relayNum > 0) && (relayNum < 17)) {
                String rlyCmd = getValue(str, ',', 3);
                if (rlyCmd == "0") {  // OFF
                  rlySta = false;
                }
                if (rlyCmd == "1") {  // ON
                  rlySta = true;
                }
                Relay(relayNum, rlySta);
                if (rlySta) {
                  bitSet(statusRelay, relayNum - 1);
                } else {
                  bitClear(statusRelay, relayNum - 1);
                }
                Serial.print("Relay ");
                Serial.print(relayNum);
                if (rlySta) {
                  Serial.println(" ON");
                } else {
                  Serial.println(" OFF");
                }
                String msg1 = String(DEVID) + ",ST," + String(statusRelay, HEX)  + "," + String(digitalRead(DIGINP));
                Serial.println(msg1);
                delay(1000);
                lastUpdate = millis();
                sendDataLora(msg1);
              }
            }
            blinkLed(3);
          }
        }
      }
    }

    // put module back to listen mode
    lora.startReceive();

    // we're ready to receive more packets,
    // enable interrupt service routine
    enableInterrupt = true;
  }

}
