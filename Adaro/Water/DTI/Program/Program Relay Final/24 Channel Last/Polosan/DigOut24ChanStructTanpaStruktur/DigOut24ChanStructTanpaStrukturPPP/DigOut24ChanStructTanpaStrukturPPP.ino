#include <RadioLib.h>
#include <IWatchdog.h>
#include <EEPROM.h>
#include "PCF8574.h"

// board STM32G030C8T6
#define LED        PC13
#define LR_RST     PA3
#define NSS        PA4
#define DIO0       PB1
#define DIO1       PA15
#define DIO2       PB3
#define LORA_PWR   PA11   // active LOW
#define ADDI2C1    0x21
#define ADDI2C2    0x22
#define ADDI2C3    0x23
#define TYPEID     2     // Relay Out 16 bit

SX1276 radio = new Module(NSS, DIO0, LR_RST, DIO1);
PCF8574 PCF1(ADDI2C1);
PCF8574 PCF2(ADDI2C2);
PCF8574 PCF3(ADDI2C3);

#define SERIAL_BAUD   115200
#define CTRBYTE    43

typedef struct {
  uint8_t configId;
  uint8_t typeId;
  uint16_t nodeId;
  uint8_t mode;
  uint8_t periode;
} Params;
Params  id;

uint32_t updateRate = 120000;
long lastUpdate;
uint32_t statusRelay = 0;
bool sendSta = false;
uint32_t channelNum;
bool changeR = false;
String strData = "Empty - Data";


String getValue(String data, char separator, int index) // 2,2,3,5
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

// save transmission states between loops
int transmissionState =  ERR_NONE;

// flag to indicate transmission or reception state
bool transmitFlag = false;

void blinkLed(int n) {
  for (int i = 0; i < n; i++) {
    digitalWrite(LED, LOW);
    delay(200);
    digitalWrite(LED, HIGH);
    delay(200);
  }
}

// flag to indicate that a packet was received
volatile bool receivedFlag = false;

// disable interrupt when it's not needed
volatile bool enableInterrupt = true;

// flag to indicate that a packet was sent or received
volatile bool operationDone = false;

// this function is called when a complete packet
// is received by the module
// IMPORTANT: this function MUST be 'void' type
//            and MUST NOT have any arguments!
void setFlag(void) {
  // check if the interrupt is enabled
  //  if (!enableInterrupt) {
  //    return;
  //  }

  // we got a packet, set the flag
  operationDone = true;
}

//Function to calculate CRC over an array 'ar' up to element 's'
byte calculateCRC(byte ar[], byte s) {
  byte rtn = 0;;
  for (byte i = 0; i < s; i++) {
    rtn ^= ar[i];
  }
  return rtn;
}

void changeRelay(uint32_t vParams) {
  vParams = vParams & 0x0FFFFFF;
  for (int i = 0; i < 24; i++) {
    if (i < 8) {
      PCF1.write(i, !bitRead(vParams, i));
    }
    if ((i > 7) && (i < 16)) {
      PCF2.write(i - 8, !bitRead(vParams, i));
    }
    if (i > 15) {
      PCF3.write(i - 16, !bitRead(vParams, i));
    }
    delay(100);
  }
  statusRelay = vParams;

  sendStatus();
}


void sendStatus() {
  delay(6000);
//  theData.typeId = id.typeId;
//  theData.nodeId = id.nodeId;
//  theData.cmd = 111;
//  theData.vParams = statusRelay & 0x0FFFFFF;
//  Serial.print(F("theData: "));
//  Serial.print(theData.typeId);
//  Serial.print(F(","));
//  Serial.print(theData.nodeId);
//  Serial.print(F(","));
//  Serial.print(theData.cmd);
//  Serial.print(F(","));
//  Serial.println(theData.vParams);
//  byte len = sizeof(theData);
//  byte byteArray[len + 1];
//  memcpy(byteArray, (const void*)(&theData), len);
//  byteArray[len] = calculateCRC(byteArray, len);
  transmissionState = radio.startTransmit(strData);
  transmitFlag = true;
  blinkLed(2);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(SERIAL_BAUD);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  pinMode(LORA_PWR, OUTPUT);
  digitalWrite(LORA_PWR, LOW);
  delay(10);

  PCF1.begin();
  PCF2.begin();
  PCF3.begin();

  if (IWatchdog.isReset(true)) {
    // LED blinks to indicate reset
    blinkLed(10);
  }

  EEPROM.get(0, id);
  if (id.configId == CTRBYTE) {
    updateRate = 1000 * id.periode;
  }
  else {
    id.configId = CTRBYTE;
    id.typeId = TYPEID;
    id.nodeId = 2;
    id.mode = 1;
    updateRate = 120 * 1000;
    id.periode = 120;
    EEPROM.put(0, id);
  }

  int state = radio.begin(920.0, 125.0, 9, 7,  SX127X_SYNC_WORD, 17, 8, 0);
  if (state ==  ERR_NONE) {
    //Serial.println(F("success!"));
  } else {
    //Serial.print(F("failed, code "));
    //Serial.println(state);
    blinkLed(20);
    while (true);
  }

  // set the function that will be called
  // when new packet is received
  radio.setDio0Action(setFlag);

  state = radio.startReceive();
  if (state ==  ERR_NONE) {
    // Serial.println(F("success!"));
  } else {
    //Serial.print(F("failed, code "));
    //Serial.println(state);
    while (true);
  }

  analogReadResolution(12);

  // Init the watchdog timer with 10 seconds timeout
  IWatchdog.begin(10000000);
  lastUpdate = millis();
  blinkLed(3);

}

void loop() {
  // put your main code here, to run repeatedly:

  IWatchdog.reload();

  if (operationDone) {
    // disable the interrupt service routine while
    // processing the data
    //enableInterrupt = false;

    // reset flag
    operationDone = false;

    if (transmitFlag) {
      // the previous operation was transmission, listen for response
      // print the result
      if (transmissionState ==  ERR_NONE) {
        // packet was successfully sent
        Serial.println(F("transmission finished!"));

      } else {
        Serial.print(F("failed, code "));
        Serial.println(transmissionState);

      }

      // listen for response
      radio.startReceive();
      transmitFlag = false;

    } else {
      String str;
      int state = radio.readData(str); //2,2,0,1#
      //str.replace("#",""); //2,2,0,1#
      if (state ==  ERR_NONE) {
        if (true) {
          if ((getValue(str,',',0).toInt() == id.typeId) && (getValue(str,',',1).toInt() == id.nodeId)) { //hasil getvalue
            int cmd = getValue(str,',',2).toInt();
            uint32_t vParams = getValue(str,',',3).toInt();
            switch (cmd) {
              case 0:
                // request status
                Serial.println("request data dieksekusi");

                sendSta = true;
                break;
              case 1:
                // change relay
                Serial.println("request changeRelay dieksekusi");
                changeR = true;
                channelNum = vParams;
                break;
              case 2:
                // ubah nodeId
                id.nodeId = vParams;
                EEPROM.put(0, id);
                break;
              case 3:
                // ubah mode
                id.mode = vParams;
                EEPROM.put(0, id);
                break;
              case 4:
                // ubah periode
                id.periode = vParams;
                updateRate = 1000 * id.periode;
                EEPROM.put(0, id);
                break;
              default:
                //
                break;
            }
          }
          blinkLed(2);
        }
      }
      radio.startReceive();
      transmitFlag = false;
    }
  }

//  if (id.mode == 1) {
//    if ((millis() - lastUpdate) > updateRate) {
//      lastUpdate = millis();
//      sendStatus(strData);
//    }
//  }

  if (sendSta) {
    sendSta = false;
    sendStatus();
  }

  if (changeR) {
    changeR = false;
    changeRelay(channelNum);
  }

}
