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
int arrayPara[24];
int duration[24];
int IntervalTimer;
String ModeRun;
bool StaRun;
bool sendSta = false;
bool runningSta = false;
bool TriggerRly = false;
bool initRun = false;
uint32_t channelNum;
bool stopSta = false;
String strData = "";
int FlagChannel[24];
int countSequence;
bool doneSequence;
int countParalel;
bool doneParalel;

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
void clearFlag() {
  for (int i = 0; i < 24; i++) {
    FlagChannel[i] = false;
  }
}
void SetDurationParalel() {
  duration[24];
  for (int i = 0; i < 24; i++) {
    duration[i] = (arrayPara[i] * 1000) + millis();
  }
}

void SetDurationSequence() {
  duration[24];
  duration[0] = (arrayPara[0] * 1000) + millis();
  for (int i = 1; i < 24; i++) {
    duration[i] = (arrayPara[i] * 1000) + duration[i - 1];
  }
}
void Sequences() { //30+15 =45
  if (!doneSequence) {
    for (int valveIndex = 0; valveIndex < 24; valveIndex++) {
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
    if (FlagChannel[23]) {
      countSequence++;
      runningSta = false;
      doneSequence = true;
    }
  }
}

void Paralel() {
  if (!doneParalel) {
    if (TriggerRly) {
      for (int i = 0; i < 24; i++) {
        CRly(i, 1);
        delay(100);
      }
      TriggerRly = false;
    }
    for (int i = 0; i < 24; i++) {
      if (millis() >= duration[i] && !FlagChannel[i]) {
        CRly(i, 0);
        FlagChannel[i] = true;
      }
    }
    bool ACF = true;
    for (int i = 0; i < 24; i++) {
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
  for (int i = 0; i < 24; i++) {
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
  delay(2000);
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
      String IncomingStr = "";
      int state = radio.readData(IncomingStr); //2,2,0,1#
      //typeId,devID,cmd,mode,interval,dura1,dura2....dura8#
      if (state ==  ERR_NONE) {
        if (true) {
          if ((getValue(IncomingStr, ',', 0).toInt() == id.typeId) && (getValue(IncomingStr, ',', 1).toInt() == id.nodeId)) { //hasil getvalue
            int cmd = getValue(IncomingStr, ',', 2).toInt();
            uint32_t vParams = getValue(IncomingStr, ',', 3).toInt();
            switch (cmd) {
              case 0:
                // request status
                Serial.println("request data dieksekusi");
                strData = String(id.typeId) + ",";
                strData += String(id.nodeId) + ",";
                strData += "111,";
                strData += "Sequence,";
                strData += String(countSequence)+",";
                strData += "Paralel,";
                strData += String(countParalel);
                sendSta = true;
                break;
              case 1: // getSetting
                if (ModeRun.length() > 0) {
                  strData = String(id.typeId) + ",";
                  strData += String(id.nodeId) + ",";
                  strData += "111,";
                  if (ModeRun == "Sequence") {
                    strData += "0,";
                  } else if (ModeRun == "Paralel") {
                    strData += "1,";
                  }
                  for (int i = 0; i < 24; i++) {
                    strData += String(arrayPara[i]);
                    if (i != 23) {
                      strData += ",";
                    }
                  }
                } else {
                  strData = String(id.typeId) + "," + String(id.nodeId) + ",111," + "1";
                }
                sendSta = true;
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
              case 5: //Setting
                if (!StaRun) { //tidak lagi run
                  if (vParams == 0) {
                    ModeRun = "Sequence";
                  } else if (vParams == 1) {
                    ModeRun = "Paralel";
                  }
                  IntervalTimer = getValue(IncomingStr, ',', 4).toInt();
                  for (int i = 5; i <= 28; i++) {
                    arrayPara[i - 5] = getValue(IncomingStr, ',', i).toInt();
                  }
                  strData = IncomingStr;
                } else {
                  strData = String(id.typeId) + "," + String(id.nodeId) + ",111,RUNNING";
                }
                sendSta = true;
                break;
              case 6:// Run
                if (ModeRun.length() > 0) {
                  StaRun = true;
                  initRun = true;
                  strData = String(id.typeId) + "," + String(id.nodeId) + ",111,RUN";
                  lastUpdate = millis();
                } else {
                  strData = String(id.typeId) + "," + String(id.nodeId) + ",111,Error";
                }

                sendSta = true;
                break;
              case 7:
                if (StaRun) {
                  StaRun = false;
                  stopSta = true;
                  strData = String(id.typeId) + "," + String(id.nodeId) + ",111,STOP";
                } else {
                  strData = String(id.typeId) + "," + String(id.nodeId) + ",111,Error";
                }
                sendSta = true;
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



  if (StaRun) {
    if ((millis() - lastUpdate) >= (IntervalTimer * 1000) || initRun) {
      initRun = false;
      lastUpdate = millis();
      runningSta = true;
      channelNum = 0;
      TriggerRly = true;
      clearFlag();
      doneSequence = false;
      doneParalel = false;
      if (ModeRun == "Sequence") {
        SetDurationSequence();
      } else if (ModeRun == "Paralel") {
        SetDurationParalel();
      }
    }
    if (runningSta) {
      if (ModeRun == "Sequence") {
        Sequences();
      } else if (ModeRun == "Paralel") {
        Paralel();
      }
    }
  } else if (stopSta) {
    stopSta = false;
    StopRly();
    doneSequence = false;
    doneParalel = false;
  }

  if (sendSta) {
    sendSta = false;
    sendStatus();
  }
}
