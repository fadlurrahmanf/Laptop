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
#define CTRBYTE    44

typedef struct {
  uint8_t configId;
  uint8_t typeId;
  uint16_t nodeId;
  uint8_t mode;
  uint8_t periode;
} Params;
Params  id;

bool StaRun;
bool sendSta;
bool runningSta;
bool TriggerRly;
bool initRun;
bool ManRly;
bool stopSta;
bool doneSequence;
bool doneParalel;
int arrayPara[20];
int duration[20];
int IntervalTimer;
int FlagChannel[20];
int countSequence;
int countParalel;
int specificRly;
int specificRlyVal;
long lastUpdate;
long UpdateSend;
uint32_t updateRate = 120000;
uint32_t channelNum;
uint32_t statusRelay = 0;
String strData = "";
String ModeRun;

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
  operationDone = true;
}
void clearFlag() {
  for (int i = 0; i < 8; i++) {
    FlagChannel[i] = false;
  }
}
void SetDurationParalel() {
  duration[8];
  for (int i = 0; i < 8; i++) {
    duration[i] = (arrayPara[i] * 1000) + millis();
  }
}

void SetDurationSequence() {
  duration[8];
  duration[0] = (arrayPara[0] * 1000) + millis();
  for (int i = 1; i < 8; i++) {
    duration[i] = (arrayPara[i] * 1000) + duration[i - 1];
  }
}
void Sequences() { //30+15 =45
  if (!doneSequence) {
    for (int valveIndex = 0; valveIndex < 8; valveIndex++) {
      if (TriggerRly && (valveIndex == 0 || FlagChannel[valveIndex - 1])) {
        CRly(valveIndex, 1);
        TriggerRly = false;
      }

      if (millis() >= duration[valveIndex] && !FlagChannel[valveIndex]) {
        CRly(valveIndex, 0);
        FlagChannel[valveIndex] = true;
        TriggerRly = true;
      }
    }
    if (FlagChannel[7]) {
      countSequence++;
      runningSta = false;
      doneSequence = true;
    }
  }
}

void Paralel() {
  if (!doneParalel) {
    if (TriggerRly) {
      for (int i = 0; i < 8; i++) {
        CRly(i, 1);
        delay(100);
      }
      TriggerRly = false;
    }
    for (int i = 0; i < 8; i++) {
      if (millis() >= duration[i] && !FlagChannel[i]) {
        CRly(i, 0);
        FlagChannel[i] = true;
        delay(100);
      }
    }
    bool ACF = true;
    for (int i = 0; i < 8; i++) {
      if (!FlagChannel[i]) {
        ACF = false;
        break;
      }
    }
    if (ACF) {
      countParalel++;
      runningSta = false;
      doneParalel = true;
    }
  }
}

void StopRly() {
  for (int i = 0; i < 8; i++) {
    CRly(i, 0);
    delay(100);
  }
}


void CRly(byte rly, bool sta) {
  sta = !sta;
  if (rly < 8) {
    PCF1.write(rly, sta);
  } else if (rly > 7 && rly < 16) {
    PCF2.write(rly - 8, sta);
  } else if (rly > 15) {
    PCF3.write(rly - 16, sta);
  }
}




void sendStatus() {
  //  delay(2000);
  if (millis() - UpdateSend >= 3000) {
    transmissionState = radio.startTransmit(strData);
    transmitFlag = true;
    sendSta = false;
    blinkLed(2);
  }
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
    id.nodeId = 208;
    id.mode = 1;
    updateRate = 120 * 1000;
    id.periode = 120;
    EEPROM.put(0, id);
  }
  strData = String(id.typeId) + "," + String(id.nodeId) + ",111," + "1";
  int state = radio.begin(921.0, 125.0, 9, 7,  SX127X_SYNC_WORD, 17, 8, 0);
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
    operationDone = false;
    if (transmitFlag) {
      if (transmissionState ==  ERR_NONE) {
        // Serial.println(F("transmission finished!"));
      } else {
//        Serial.print(F("failed, code "));
      }
      transmitFlag = false;
    } else {
      transmitFlag = false;
    }
  }
  if (sendSta) {
    sendStatus();
  }

}
