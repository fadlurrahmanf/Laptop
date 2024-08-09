#include <RadioLib.h>
#include <IWatchdog.h>
#include <EEPROM.h>
#include "PCF8574.h"

#define LED        PC13
#define LR_RST     PB14
#define NSS        PA4
#define DIO0       PB1
#define DIO1       PA15
#define DIO2       PB3
#define LORA_ON    PA11   // active LOW
#define ADDI2C1    0x21
#define ADDI2C2    0x23

SX1276 lora = new Module(NSS, DIO0, LR_RST, DIO1);

String DEVID = "RLY104";
uint32_t updateRate = 120000;
long lastUpdate;
bool transmitDone = false;
uint16_t statusRelay = 0;

PCF8574 PCF1(ADDI2C1);
PCF8574 PCF2(ADDI2C2);

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

  transmitDone = false;
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

  if (IWatchdog.isReset(true)) {
    // LED blinks to indicate reset
    blinkLed(10);
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

  blinkLed(3);

}

void loop() {
  // put your main code here, to run repeatedly:

  IWatchdog.reload();

  long now = millis();
  if ((now - lastUpdate) > updateRate) {
    lastUpdate = now;
    String msg = String(DEVID) + ",ST," + String(statusRelay, HEX);
    sendDataLora(msg);
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
          String devid = getValue(str, ',', 0);

          if (devid == DEVID) {
            String rlyNum = getValue(str, ',', 1);
            int relayNum = rlyNum.toInt();
            if ((relayNum > 0) && (relayNum < 17)) {
              String rlyCmd = getValue(str, ',', 2);
              if (rlyCmd == "0") {  // OFF
                rlySta = false;
              }
              if (rlyCmd == "1") {  // ON
                rlySta = true;
              }
              Relay(relayNum, rlySta);
              if (rlySta) {
                bitSet(statusRelay,relayNum - 1);
              } else {
                bitClear(statusRelay,relayNum - 1);
              }
              Serial.print("Relay ");
              Serial.print(relayNum);
              if (rlySta) {
                Serial.println(" ON");
              } else {
                Serial.println(" OFF");
              }
              String msg1 = String(DEVID) + ",ST," + String(statusRelay,HEX);
              
              Serial.println(msg1);
              delay(1000);
              lastUpdate = millis();
              sendDataLora(msg1);
              blinkLed(3);
            }
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
