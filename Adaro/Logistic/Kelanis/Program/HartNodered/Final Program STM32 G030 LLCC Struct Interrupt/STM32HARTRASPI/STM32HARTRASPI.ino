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
#define CTRBYTE 0x50

LLCC68 radio = new Module(NSS, DIO1, NRST, BUSY);

// save transmission states between loops
int transmissionState = RADIOLIB_ERR_NONE;

// flag to indicate transmission or reception state
bool transmitFlag = false;

// flag to indicate that a packet was sent or received
volatile bool operationDone = false;


String flow = "1", totalizer = "2", density = "3", temperature = "4";  // Variabel untuk menyimpan parameter yang diparsing
unsigned long previousMillis = 0;
const long interval = 60000;  // interval at which to blink (milliseconds)
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

int dsp;

void parseData(String data) {
  int commaIndex1 = data.indexOf(',');                   // Cari posisi koma pertama
  int commaIndex2 = data.indexOf(',', commaIndex1 + 1);  // Cari posisi koma kedua
  int commaIndex3 = data.indexOf(',', commaIndex2 + 1);  // Cari posisi koma ketiga

  if (commaIndex1 >= 0 && commaIndex2 >= 0 && commaIndex3 >= 0) {  // Pastikan ada 3 koma
    flow = data.substring(0, commaIndex1);
    totalizer = data.substring(commaIndex1 + 1, commaIndex2);
    density = data.substring(commaIndex2 + 1, commaIndex3);
    temperature = data.substring(commaIndex3 + 1);
  }
}

void sendStatus() {
  theData.typeId = id.typeId;
  theData.nodeId = id.nodeId;
  if (dsp == 0) {
    theData.cmd = 10;
    theData.vParams = (int)(flow.toFloat() * 10);
  }
  if (dsp == 1) {
    theData.cmd = 11;
    theData.vParams = (int)(totalizer.toFloat() * 10);
  }
  if (dsp == 2) {
    theData.cmd = 12;
    theData.vParams = (int)(density.toFloat() * 1000);
  }
  if (dsp == 3) {
    theData.cmd = 13;
    theData.vParams = (int)(temperature.toFloat() * 10);
  }
  byte len = sizeof(theData);
  byte byteArray[len + 1];
  memcpy(byteArray, (const void*)(&theData), len);
  byteArray[len] = calculateCRC(byteArray, len);
  transmissionState = radio.startTransmit(byteArray, len + 1);
  transmitFlag = true;
  blinkLed(2);
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

  EEPROM.get(0, id);
  if (id.configId == CTRBYTE) {

  } else {
    id.configId = CTRBYTE;
    id.typeId = TYPEID;
    id.nodeId = 8;
    id.modeSta = 1;
    id.periode = 30;
    EEPROM.put(0, id);
  }

  int state = radio.begin(920.0, 125.0, 9, 7, RADIOLIB_SX127X_SYNC_WORD, 17, 8, 0);
  if (state == RADIOLIB_ERR_NONE) {
    //Serial.println(F("success!"));
  } else {
    //Serial.print(F("failed, code "));
    //Serial.println(state);
    blinkLed(20);
    while (true)
      ;
  }
  radio.setDio1Action(setFlag);

  state = radio.startReceive();
  if (state == RADIOLIB_ERR_NONE) {
    // Serial.println(F("success!"));
  } else {
    //Serial.print(F("failed, code "));
    //Serial.println(state);
    while (true)
      ;
  }
  
  blinkLed(2);
}

void loop() {
  if (operationDone) {

    // reset flag
    operationDone = false;
    if (transmitFlag) {
      // the previous operation was transmission, listen for response
      // print the result
      if (transmissionState == RADIOLIB_ERR_NONE) {
        // packet was successfully sent
        //Serial.println(F("transmission finished!"));
        blinkLed(2);

      } else {
        //Serial.print(F("failed, code "));
        //Serial.println(transmissionState);
      }
      radio.startReceive();
      transmitFlag = false;
    } else {
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
        }
      }
      blinkLed(2);
      radio.startReceive();
      transmitFlag = false;
    }
  }

  if (Serial.available() > 0) {
    String receivedData = Serial.readStringUntil('#');
    parseData(receivedData);
    blinkLed(1);
  }
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval || sendSta) {
    sendSta = false;
    blinkLed(1);
    previousMillis = currentMillis - random(5000);
    sendStatus();
    dsp++;
    if (dsp > 3) {
      dsp = 0;
    }
  }
}
