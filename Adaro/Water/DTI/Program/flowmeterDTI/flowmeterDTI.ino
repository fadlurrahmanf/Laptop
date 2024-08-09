#include <RadioLib.h>
#include <IWatchdog.h>
#include <EEPROM.h>
#include <GyverINA.h>

#define CTRBYTE       44
// board STM32G030C8T6DcV2
//#define LR_RST     PA3
//#define NSS        PA4
//#define DIO0       PB3
//#define DIO1       PA15
//#define DIO2       PA12
//#define LED        PC13
//#define LORA_PWR   PA11

// board STM32G030C8T6LoRaCurrentLoop
// #define LR_RST     PA3
// #define NSS        PA4
// #define DIO0       PB4
// #define DIO1       PB3
// #define DIO2       PA15
// #define LED        PC13
//#define LORA_PWR   PA11

// board loraModuleL071ModuleV2
#define LR_RST     PC4
#define NSS        PA4
#define DIO0       PB5
#define DIO1       PB4
#define DIO2       PB3
#define LED        PC13
#define LORA_PWR   PB8

#define COUNT_PIN  PB12

SX1276 radio = new Module(NSS, DIO0, LR_RST, DIO1);
INA226 ina(3.6f, 0.02f, 0x40);

#define SERIAL_BAUD   115200
#define TYPEID        11   // current loop sensor

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
  uint8_t modeSta;
  uint8_t periode;
  long cacahan;
} Params;
Params  id;

long lastUpdate;
bool updateSta = false;
int updateRate = 2 * 60 * 1000; // update rate 2 menit
bool sendSta = false;
long standMeter = 0;
bool sendPara = false;

// save transmission states between loops
int transmissionState = RADIOLIB_ERR_NONE;

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

void sendStatus(bool sendPara) {
  delay(1000);
  theData.typeId = id.typeId;
  theData.nodeId = id.nodeId;
  if (sendPara) {
    theData.cmd = 111;
    theData.vParams = (ina.getCurrent()*100000);
  } else
  {
    theData.cmd = 112;
    theData.vParams = standMeter;
  }
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

void cacah() {
  standMeter++;
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(SERIAL_BAUD);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  pinMode(LORA_PWR, OUTPUT);
  digitalWrite(LORA_PWR, LOW);
  delay(10);

  if (IWatchdog.isReset(true)) {
    // LED blinks to indicate reset
    blinkLed(10);
  }

  EEPROM.get(0, id);
  if (id.configId == CTRBYTE) {
    updateRate = 1000 * id.periode;
    standMeter = id.cacahan;
  }
  else {
    id.configId = CTRBYTE;
    id.typeId = TYPEID;
    id.nodeId = 2;
    id.modeSta = 1;
    updateRate = 120 * 1000;
    id.periode = 120;
    id.cacahan = 0;
    EEPROM.put(0, id);
  }

  int state = radio.begin(920.0, 125.0, 9, 7, RADIOLIB_SX127X_SYNC_WORD, 17, 8, 0);
  if (state == RADIOLIB_ERR_NONE) {
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
  if (state == RADIOLIB_ERR_NONE) {
    // Serial.println(F("success!"));
  } else {
    //Serial.print(F("failed, code "));
    //Serial.println(state);
    while (true);
  }

  analogReadResolution(12);

  if (ina.begin()) {
    Serial.println(F("connected!"));
  } else {
    Serial.println(F("not found!"));
    while (1);
  }

  Serial.print(F("Calibration value: ")); Serial.println(ina.getCalibration());
  ina.setSampleTime(INA226_VBUS, INA226_CONV_2116US);
  ina.setSampleTime(INA226_VSHUNT, INA226_CONV_8244US);
  ina.setAveraging(INA226_AVG_X128);
  
  // Init the watchdog timer with 10 seconds timeout
  IWatchdog.begin(10000000);
  lastUpdate = millis();
  attachInterrupt(COUNT_PIN, cacah, RISING);
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

      byte len = sizeof(theData);
      byte byteArray[len + 1];
      int state = radio.readData(byteArray, len + 1);
      if (state == RADIOLIB_ERR_NONE) {
        theData = *(Payload*)byteArray;
        if (byteArray[len] == calculateCRC(byteArray, len)) {
          if ((theData.typeId == id.typeId) && (theData.nodeId == id.nodeId)) {
            int cmd = theData.cmd;
            int vParams = theData.vParams;
            switch (cmd) {
              case 0:
                // request status
                Serial.println("request dieksekusi");
                if (vParams == 0) {
                  sendPara = false;
                } else {
                  sendPara = true;
                }
                sendSta = true;
                break;
              case 1:
                break;
              case 2:
                // ubah nodeId
                id.nodeId = vParams;
                EEPROM.put(0, id);
                break;
              case 3:
                // ubah mode
                id.modeSta = vParams;
                EEPROM.put(0, id);
                break;
              case 4:
                // ubah periode
                id.periode = vParams;
                updateRate = 1000 * id.periode;
                EEPROM.put(0, id);
                break;
              case 5:
                // ubah periode
                id.cacahan = vParams;
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
    //enableInterrupt = true;
  }

  if (id.modeSta == 1) {
    if ((millis() - lastUpdate) > updateRate) {
      lastUpdate = millis();
      sendPara = !sendPara;
      sendStatus(sendPara);
    }
  }

  if (sendSta) {
    sendSta = false;
    sendStatus(sendPara);
  }
}
