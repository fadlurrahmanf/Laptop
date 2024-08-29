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

#define OUT1OFF PB6  //Output1
#define OUT1ON PB7  //Output1
#define OUT2OFF PB8 //Output2
#define OUT2ON PB9  //Output2
#define PIN1 PC15  // Input1
#define PIN2 PC14  // Input2

#define CTRBYTE 0x04

uint8_t updatePeriode = 120;
uint32_t updateRateSend = updatePeriode * 1000;
long lastUpdate;
uint8_t statusRelay = 0;
bool sendSta = false;
uint8_t channelNum;
bool changeR = false;
uint8_t mode = 0;
uint8_t deviceId = 4;
uint8_t networkId = 4;
uint8_t typeId = 4;
uint8_t lastRead = 0x03;

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
  for (int i = 0; i < 2; i++) {
    switch (i) {
      case 0:
        if(bitRead(vParams, i)){
          digitalWrite(OUT1ON, HIGH);
          delay(100);
          digitalWrite(OUT1ON, LOW);
        }else{
          digitalWrite(OUT1OFF, HIGH);
          delay(100);
          digitalWrite(OUT1OFF, LOW);
        }
        delay(100);
        break;
      case 1:
        if(bitRead(vParams, i)){
          digitalWrite(OUT2ON, HIGH);
          delay(100);
          digitalWrite(OUT2ON, LOW);
        }else{
          digitalWrite(OUT2OFF, HIGH);
          delay(100);
          digitalWrite(OUT2OFF, LOW);
        }
        delay(100);
      default:
        break;
    }
  }
  statusRelay = vParams & 0x03;
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
  uint8_t currentRead = 0x03;
  uint8_t sum = 0;
  for (int i = 0; i < 2; i++) {
    switch (i) {
      case 0:
        sum = 0;
        for (int a = 0; a < 10; a++) {
          sum += digitalRead(PIN1);
          delay(10);
        }
        if (sum < 8) {
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
        if (sum < 8) {
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

  pinMode(OUT1OFF, OUTPUT);
  pinMode(OUT1ON, OUTPUT);
  pinMode(OUT2OFF, OUTPUT);
  pinMode(OUT2ON, OUTPUT);
  pinMode(PIN1, INPUT);
  pinMode(PIN2, INPUT_PULLUP);

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
          }
          blinkLed(2);
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
