#include <RadioLib.h>
#include <EEPROM.h>


#define HAL_PWR_MODULE_ENABLED

// board G030C8T6
#define LED PC15
#define NSS PA4
#define DIO1 PD0
#define NRST PD2
#define BUSY PD1
#define SERIAL_BAUD 115200
#define TYPEID 7
#define CTRBYTE 0x47

LLCC68 radio = new Module(NSS, DIO1, NRST, BUSY);

// save transmission states between loops
int transmissionState = RADIOLIB_ERR_NONE;

// flag to indicate transmission or reception state
bool transmitFlag = false;

// flag to indicate that a packet was sent or received
volatile bool operationDone = false;

uint32_t updateRate = 6000;
long lastUpdate;
bool sendSta = false;

typedef struct {
  uint8_t typeId;
  uint16_t nodeId;   //store this nodeId
  uint8_t cmd;       //type parameter
  uint32_t vParams;  //temperature maybe?
} Payload;
Payload theData;

typedef struct {
  uint8_t configId;
  uint8_t typeId;
  uint16_t nodeId;
  uint8_t modeSta;
  uint8_t periode;
} Params;
Params id;

void setFlag(void) {
  // we sent or received a packet, set the flag
  operationDone = true;
}

void blinkLed(int n) {
  for (int i = 0; i < n; i++) {
    digitalWrite(LED, LOW);
    delay(200);
    digitalWrite(LED, HIGH);
    delay(200);
  }
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

void sendStatus() {
  theData.typeId = id.typeId;
  theData.nodeId = id.nodeId;
  theData.cmd = 10;
  float current_mA = 0.0;
  current_mA = 1 * 100 - 20;
  theData.vParams = current_mA;
  byte len = sizeof(theData);
  byte byteArray[len + 1];
  memcpy(byteArray, (const void*)(&theData), len);
  byteArray[len] = calculateCRC(byteArray, len);
  transmissionState = radio.startTransmit(byteArray, len + 1);
  transmitFlag = true;
  blinkLed(2);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

  //enableBackupDomain();

  EEPROM.get(0, id);
  if (id.configId == CTRBYTE) {

  } else {
    id.configId = CTRBYTE;
    id.typeId = TYPEID;
    id.nodeId = 7;
    id.modeSta = 1;
    id.periode = 30;
    EEPROM.put(0, id);
  }

  int state = radio.begin(920.0, 125.0, 9, 7, RADIOLIB_SX127X_SYNC_WORD, 17, 8, 0);
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("LORA success!"));
  } else {
    Serial.print(F("LORA failed, code "));
    Serial.println(state);
    blinkLed(20);
    while (true)
      ;
  }
  radio.setDio1Action(setFlag);
  state = radio.startReceive();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("State success!"));
  } else {
    Serial.print(F("State failed, code "));
    Serial.println(state);
    blinkLed(10);
    while (true)
      ;
  }
  analogReadResolution(12);
  blinkLed(2);
}

void loop() {
  

  if (operationDone) {
    Serial.println("Lora Activity Detected");
    // disable the interrupt service routine while
    // processing the data
    //enableInterrupt = false;

    // reset flag
    operationDone = false;

    if (transmitFlag) {
      Serial.println("Transmit");
      // the previous operation was transmission, listen for response
      // print the result
      if (transmissionState == RADIOLIB_ERR_NONE) {
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
      Serial.println("Receive");
      byte len = sizeof(theData);
      byte byteArray[len + 1];
      int state = radio.readData(byteArray, len + 1);
      if (state == RADIOLIB_ERR_NONE) {
        theData = *(Payload*)byteArray;
        if (byteArray[len] == calculateCRC(byteArray, len)) {
          if ((theData.typeId == id.typeId) && (theData.nodeId == id.nodeId)) {
            int cmd = theData.cmd;
            uint32_t vParams = theData.vParams;
            switch (cmd) {
              case 0:
                // request status
                Serial.println("request data dieksekusi");

                sendSta = true;
                break;
              case 1:
                // change relay
                Serial.println("request changeRelay dieksekusi");
                // changeR = true;
                // channelNum = vParams;
                break;
              case 2:
                // ubah nodeId
                id.nodeId = vParams;
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
  if (sendSta) {
    sendSta = false;
    sendStatus();
  }
  if ((millis() - lastUpdate) > updateRate) {
    blinkLed(1);
    lastUpdate = millis();
    sendStatus();
  }
}
