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
#define ADDI2C1    0x20
#define ADDI2C2    0x21
#define ADDI2C3    0x22
#define TYPEID     3     // Digital Input 24 bit

SX1276 radio = new Module(NSS, DIO0, LR_RST, DIO1);
PCF8574 PCF1(ADDI2C1);
PCF8574 PCF2(ADDI2C2);
PCF8574 PCF3(ADDI2C3);

#define SERIAL_BAUD   115200
#define CTRBYTE    41

typedef struct {
  uint8_t       typeId;
  uint16_t      nodeId; //store this nodeId
  uint8_t       cmd; //type parameter
  uint32_t      vParams;   //temperature maybe?
} Payload;
Payload theData;

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
uint32_t statusInput = 0;
bool sendSta = false;

// save transmission states between loops
int transmissionState = ERR_NONE;

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

uint32_t updateStatus() {
  uint8_t a = PCF1.read8();
  uint8_t b = PCF2.read8();
  uint8_t c = PCF3.read8();
  uint32_t statInput = a + b*256 + c*256*256;
  statInput = (~statInput) & 0x00FFFFFF;
  return statInput;
}

void sendStatus() {
  theData.typeId = id.typeId;
  theData.nodeId = id.nodeId;
  theData.cmd = 111;
  theData.vParams = statusInput;
  Serial.print(F("theData: "));
  Serial.print(theData.typeId);
  Serial.print(F(","));
  Serial.print(theData.nodeId);
  Serial.print(F(","));
  Serial.print(theData.cmd);
  Serial.print(F(","));
  Serial.println(theData.vParams);
  byte len = sizeof(theData);
  byte byteArray[len + 1];
  memcpy(byteArray, (const void*)(&theData), len);
  byteArray[len] = calculateCRC(byteArray, len);
  transmissionState = radio.startTransmit(byteArray, len + 1);
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

  int state = radio.begin(920.0, 125.0, 9, 7, SX127X_SYNC_WORD, 17, 8, 0);
  if (state == ERR_NONE) {
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
  if (state == ERR_NONE) {
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
      if (transmissionState == ERR_NONE) {
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

      byte len = sizeof(theData);
      byte byteArray[len + 1];
      int state = radio.readData(byteArray, len + 1);
      if (state == ERR_NONE) {
        theData = *(Payload*)byteArray;
        if (byteArray[len] == calculateCRC(byteArray, len)) {
          if ((theData.typeId == id.typeId) && (theData.nodeId == id.nodeId)) {
            int cmd = theData.cmd;
            int vParams = theData.vParams;
            switch (cmd) {
              case 0:
                // request status
                Serial.println("request data dieksekusi");
                delay(2000);
                sendSta = true;
                break;
              case 1:
                // change relay
//                Serial.println("request changeRelay dieksekusi");
//                changeRelay(vParams);
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

  if (id.mode == 1) {
    if ((millis() - lastUpdate) > updateRate) {
      lastUpdate = millis();
      sendStatus();
    }
  }
  uint32_t ab = updateStatus();
  if (statusInput != ab) {
    statusInput = ab;
    sendSta = true;
  }

  if (sendSta) {
    sendSta = false;
    sendStatus();
  }

}
