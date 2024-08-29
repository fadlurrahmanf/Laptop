#include <RadioLib.h>
#include <IWatchdog.h>
#include <EEPROM.h>

// Board PCB_STM32G030C8T6
#define LED PC13
#define NSS PA4
#define DIO0 PB1
#define DIO1 PA15
#define NRST PA3
#define LORA_ON PA11

#define OUT1 PA10
#define OUT2 PA9
#define OUT3 PA8
#define OUT4 PB15
#define OUT5 PB13
#define OUT6 PB12
#define OUT7 PB11
#define OUT8 PB10

#define PIN1 PB6   // INPUT1
#define PIN2 PB7   // INPUT2
#define PIN3 PB8   // INPUT3
#define PIN4 PB9   // INPUT4
#define PIN5 PC14  // INPUT5
#define PIN6 PC15  // INPUT6
#define PIN7 PB4   // INPUT7
#define PIN8 PB5   // INPUT8

#define CTRBYTE 0x03

uint8_t updatePeriode = 120;
uint32_t updateRateSend = updatePeriode * 1000;
long lastUpdate;
uint8_t statusRelay = 0;
bool sendSta = false;
uint8_t channelNum;
bool changeR = false;
uint8_t mode = 0;
uint8_t deviceId = 3;
uint8_t networkId = 4;
uint8_t typeId = 4; //Relay 8 Channel
uint8_t lastRead = 0;

SX1276 radio = new Module(NSS, DIO0, NRST, DIO1);

typedef struct {
  uint8_t networkId;
  uint8_t deviceId;  //store this nodeId
  uint8_t cmd;       //type parameter
  uint32_t vParams;  //temperature maybe?
} Payload;
Payload theData;

typedef struct {
  uint8_t configId;
  uint8_t typeId;
  uint8_t networkId;
  uint16_t deviceId;
  uint8_t mode;
  uint8_t periode;
} Params;
Params id;



void blinkLed(int n) {
  for (int i = 0; i < n; i++) {
    digitalWrite(LED, LOW);
    delay(200);
    digitalWrite(LED, HIGH);
    delay(200);
  }
}

// save transmission states between loops
int transmissionState = RADIOLIB_ERR_NONE;

// flag to indicate transmission or reception state
bool transmitFlag = false;

// flag to indicate that a packet was sent or received
volatile bool operationDone = false;

// this function is called when a complete packet
// is received by the module
// IMPORTANT: this function MUST be 'void' type
//            and MUST NOT have any arguments!
void setFlag(void) {
  // we got a packet, set the flag
  operationDone = true;
}

//Function to calculate CRC over an array 'ar' up to element 's'
byte calculateCRC(byte ar[], byte s) {
  byte rtn = 0;
  ;
  for (byte i = 0; i < s; i++) {
    rtn ^= ar[i];
  }
  return rtn;
}

void changeRelay(uint8_t vParams) {
  vParams = vParams & 0x0FF;
  for (int i = 0; i < 8; i++) {
    switch (i) {
      case 0:
        digitalWrite(OUT1, bitRead(vParams, i));
        delay(100);
        break;
      case 1:
        digitalWrite(OUT2, bitRead(vParams, i));
        delay(100);
        break;
      case 2:
        digitalWrite(OUT3, bitRead(vParams, i));
        delay(100);
        break;
      case 3:
        digitalWrite(OUT4, bitRead(vParams, i));
        delay(100);
        break;
      case 4:
        digitalWrite(OUT5, bitRead(vParams, i));
        delay(100);
        break;
      case 5:
        digitalWrite(OUT6, bitRead(vParams, i));
        delay(100);
        break;
      case 6:
        digitalWrite(OUT7, bitRead(vParams, i));
        delay(100);
        break;
      case 7:
        digitalWrite(OUT8, bitRead(vParams, i));
        delay(100);
        break;
      default:
        break;
    }
  }
  statusRelay = vParams;
  delay(2000);
}


void sendStatus() {
  theData.networkId = id.networkId;
  theData.deviceId = id.deviceId;
  theData.vParams = statusRelay * 256 + lastRead;
  byte len = sizeof(theData);
  byte byteArray[len + 1];
  memcpy(byteArray, (const void *)(&theData), len);
  byteArray[len] = calculateCRC(byteArray, len);
  transmissionState = radio.startTransmit(byteArray, len + 1);
  transmitFlag = true;
  blinkLed(2);
}
uint8_t checkInput() {
  uint8_t currentRead = 0xFF;
  uint8_t sum = 0;
  for (int i = 0; i < 8; i++) {
    switch (i) {
      case 0:
        sum = 0;
        for (int a = 0; a < 10; a++) {
          sum += digitalRead(PIN1);
          delay(10);
        }
        if (sum > 8) {
          currentRead = bitSet(currentRead, i);
        } else {
          currentRead = bitClear(currentRead, i);
        }
        break;
      case 1:
        sum = 0;
        for (int a = 0; a < 10; a++) {
          sum += digitalRead(PIN2);
          delay(10);
        }
        if (sum > 8) {
          currentRead = bitSet(currentRead, i);
        } else {
          currentRead = bitClear(currentRead, i);
        }
        break;
      case 2:
        sum = 0;
        for (int a = 0; a < 10; a++) {
          sum += digitalRead(PIN3);
          delay(10);
        }
        if (sum > 8) {
          currentRead = bitSet(currentRead, i);
        } else {
          currentRead = bitClear(currentRead, i);
        }
        break;
      case 3:
        sum = 0;
        for (int a = 0; a < 10; a++) {
          sum += digitalRead(PIN4);
          delay(10);
        }
        if (sum > 8) {
          currentRead = bitSet(currentRead, i);
        } else {
          currentRead = bitClear(currentRead, i);
        }
        break;
      case 4:
        sum = 0;
        for (int a = 0; a < 10; a++) {
          sum += digitalRead(PIN5);
          delay(10);
        }
        if (sum > 8) {
          currentRead = bitSet(currentRead, i);
        } else {
          currentRead = bitClear(currentRead, i);
        }
        break;
      case 5:
        sum = 0;
        for (int a = 0; a < 10; a++) {
          sum += digitalRead(PIN6);
          delay(10);
        }
        if (sum > 8) {
          currentRead = bitSet(currentRead, i);
        } else {
          currentRead = bitClear(currentRead, i);
        }
        break;
      case 6:
        sum = 0;
        for (int a = 0; a < 10; a++) {
          sum += digitalRead(PIN7);
          delay(10);
        }
        if (sum > 8) {
          currentRead = bitSet(currentRead, i);
        } else {
          currentRead = bitClear(currentRead, i);
        }
        break;
      case 7:
        sum = 0;
        for (int a = 0; a < 10; a++) {
          sum += digitalRead(PIN8);
          delay(10);
        }
        if (sum > 8) {
          currentRead = bitSet(currentRead, i);
        } else {
          currentRead = bitClear(currentRead, i);
        }
        break;
      default:
        break;
    }
  }
  return currentRead;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  pinMode(LORA_ON, OUTPUT);
  digitalWrite(LORA_ON, LOW);
  delay(10);

  pinMode(OUT1, OUTPUT);
  pinMode(OUT2, OUTPUT);
  pinMode(OUT3, OUTPUT);
  pinMode(OUT4, OUTPUT);
  pinMode(OUT5, OUTPUT);
  pinMode(OUT6, OUTPUT);
  pinMode(OUT7, OUTPUT);
  pinMode(OUT8, OUTPUT);

  pinMode(PIN1, INPUT);
  pinMode(PIN2, INPUT);
  pinMode(PIN3, INPUT);
  pinMode(PIN4, INPUT);
  pinMode(PIN5, INPUT);
  pinMode(PIN6, INPUT);
  pinMode(PIN7, INPUT);
  pinMode(PIN8, INPUT);

  if (IWatchdog.isReset(true)) {
    // LED blinks to indicate reset
    blinkLed(10);
  }

  EEPROM.get(0, id);
  if (id.configId == CTRBYTE) {
    networkId = id.networkId;
    deviceId = id.deviceId;
    updatePeriode = id.periode;
    mode = id.mode;
    updateRateSend = 1000 * id.periode;
  } else {
    id.configId = CTRBYTE;
    id.networkId = networkId;
    id.deviceId = deviceId;
    id.mode = mode;
    id.periode = updatePeriode;
    updateRateSend = updatePeriode * 1000;
    EEPROM.put(0, id);
  }

  int state = radio.begin(921.0, 125.0, 9, 7, RADIOLIB_SX127X_SYNC_WORD, 17, 8, 0);
  if (state == RADIOLIB_ERR_NONE) {
    //Serial.println(F("success!"));
  } else {
    //Serial.print(F("failed, code "));
    //Serial.println(state);
    blinkLed(20);
    while (true)
      ;
  }

  // set the function that will be called
  // when new packet is received
  radio.setDio0Action(setFlag, RISING);

  state = radio.startReceive();
  if (state == RADIOLIB_ERR_NONE) {
    // Serial.println(F("success!"));
  } else {
    //Serial.print(F("failed, code "));
    //Serial.println(state);
    while (true)
      ;
  }

  // Init the watchdog timer with 10 seconds timeout
  IWatchdog.begin(10000000);
  lastUpdate = millis();
  blinkLed(3);
}

void loop() {
  // put your main code here, to run repeatedly:

  IWatchdog.reload();

  uint8_t currentRead = checkInput();
  if (currentRead != lastRead) {
    lastRead = currentRead;
    sendStatus();
  }

  if (operationDone) {

    // reset flag
    operationDone = false;

    if (transmitFlag) {
      // the previous operation was transmission, listen for response
      // print the result
      if (transmissionState == RADIOLIB_ERR_NONE) {
        // packet was successfully sent
        // Serial.println(F("transmission finished!"));

      } else {
        // Serial.print(F("failed, code "));
        // Serial.println(transmissionState);
      }

      // listen for response
      radio.startReceive();
      transmitFlag = false;

    } else {

      size_t len = radio.getPacketLength();
      uint8_t byteArray[len];
      int state = radio.readData(byteArray, len);

      if (state == RADIOLIB_ERR_NONE) {
        if (len < 8) {
          if ((byteArray[0] == id.networkId) && (byteArray[1] == id.deviceId)) {
            int cmd = byteArray[2];
            uint32_t vParams = byteArray[6];
            // uint32_t vParams = byteArray[3]<<24 + byteArray[4]<<16 + byteArray[5]<<8 + byteArray[6];
            switch (cmd) {
              case 0:
                // request status

                sendSta = true;
                break;
              case 1:
                // change relay

                changeR = true;
                channelNum = vParams;
                break;
              case 2:
                // ubah nodeId
                id.networkId = vParams;
                EEPROM.put(0, id);
                break;
              case 3:
                // ubah nodeId
                id.deviceId = vParams;
                EEPROM.put(0, id);
                break;
              case 4:
                // ubah mode
                id.mode = vParams;
                EEPROM.put(0, id);
                break;
              case 5:
                // ubah periode
                id.periode = vParams;
                updateRateSend = 1000 * id.periode;
                EEPROM.put(0, id);
                break;
              default:
                //
                break;
            }
            blinkLed(1);
          }
          blinkLed(1);
        }
      }
      radio.startReceive();
      transmitFlag = false;
    }
  }

  if (id.mode == 1) {
    if ((millis() - lastUpdate) > updateRateSend) {
      lastUpdate = millis();
      sendStatus();
    }
  }

  if (sendSta) {
    sendSta = false;
    delay(2000);
    sendStatus();
  }

  if (changeR) {
    changeR = false;
    changeRelay(channelNum);
    delay(2000);
    uint8_t currentRead = checkInput();
    if (currentRead != lastRead) {
      lastRead = currentRead;
    }
    sendStatus();
  }
}