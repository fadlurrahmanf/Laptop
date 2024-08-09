#include <RadioLib.h>
#include <IWatchdog.h>
#include <EEPROM.h>

// Board STM32G030C8T6LLCC68
#define LED PC15
#define NSS PA4
#define DIO1 PD0
#define NRST PD2
#define BUSY PD1
#define VBAT PA0

#define CTRBYTE 0x41
#define GSM_ON PB6

#define GSM_TIMER 10 * 60 * 1000  // 10 menit tunggu GSM

LLCC68 radio = new Module(NSS, DIO1, NRST, BUSY);

typedef struct {
  uint8_t typeId;
  uint16_t nodeId;   //store this nodeId
  uint8_t cmd;       //type parameter
  uint32_t vParams;  //temperature maybe?
} Payload;
Payload theData;

typedef struct {
  uint8_t configid;
  uint8_t devid;
} Params;
Params id;

String DEVID = "AWP01";
long lastUpdate;
bool updateSta = false;
int countUpdate = 0;
long lastUpdateGSM = millis();
bool gsmOnSta = true;

void blinkLed(int n) {
  for (int i = 0; i < n; i++) {
    digitalWrite(LED, LOW);
    delay(200);
    digitalWrite(LED, HIGH);
    delay(200);
  }
}

String xorChecksum(String s) {
  byte b = s.charAt(0);
  for (int i = 1; i < s.length(); i++) {
    b = b ^ s.charAt(i);
  }
  String checksum = String(b, HEX);
  if (checksum.length() == 1) checksum = "0" + checksum;
  return checksum;
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

String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
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
int transmissionState = ERR_NONE;

// flag to indicate transmission or reception state
bool transmitFlag = false;

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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(GSM_ON, OUTPUT);
  pinMode(VBAT, INPUT);
  digitalWrite(GSM_ON, LOW);
  gsmOnSta = true;
  lastUpdateGSM = millis();

  if (IWatchdog.isReset(true)) {
    // LED blinks to indicate reset
    blinkLed(10);
  }

  EEPROM.get(0, id);
  if (id.configid == CTRBYTE) {
    DEVID = "GW" + String(id.devid);
  } else {
    id.configid = CTRBYTE;
    id.devid = 76;
    DEVID = "GW76";
    EEPROM.put(0, id);
  }

  int state = radio.begin(920.0, 125.0, 9, 7, SX127X_SYNC_WORD, 17, 8, 0);
  if (state == ERR_NONE) {
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
  radio.setDio1Action(setFlag);

  state = radio.startReceive();
  if (state == ERR_NONE) {
    // Serial.println(F("success!"));
  } else {
    //Serial.print(F("failed, code "));
    //Serial.println(state);
    while (true)
      ;
  }

  // Init the watchdog timer with 10 seconds timeout
  analogReadResolution(12);
  IWatchdog.begin(10000000);
  lastUpdate = millis();
  blinkLed(3);
}

void loop() {
  // put your main code here, to run repeatedly:
  IWatchdog.reload();

  // if (gsmOnSta) {
  //   if ((millis() - lastUpdateGSM) > GSM_TIMER) {
  //     // OFF GSM
  //     digitalWrite(GSM_ON, LOW);
  //     gsmOnSta = false;
  //   }
  // }

  if (Serial.available() > 0) {
    String str = Serial.readStringUntil('#');  //2,2,3,4#
    String tmp = getValue(str, ',', 0);
    if (tmp == "GW") {
      String dvid = getValue(str, ',', 1);
      DEVID = "GW" + dvid;
      id.devid = dvid.toInt();
      EEPROM.put(0, id);
    } else {
      String typeIDStr = getValue(str, ',', 0);
      uint8_t typeID = typeIDStr.toInt();
      String destIDStr = getValue(str, ',', 1);
      uint16_t destID = destIDStr.toInt();
      String cmdStr = getValue(str, ',', 2);
      uint8_t cmd = cmdStr.toInt();
      String paramStr = getValue(str, ',', 3);
      uint32_t vParams = paramStr.toInt();
      theData.typeId = typeID;
      theData.nodeId = destID;
      theData.cmd = cmd;
      theData.vParams = vParams;
      byte len = sizeof(theData);
      byte byteArray[len + 1];
      memcpy(byteArray, (const void*)(&theData), len);
      byteArray[len] = calculateCRC(byteArray, len);
      transmissionState = radio.startTransmit(byteArray, len + 1);
      transmitFlag = true;
      //Serial.begin(115200);
    }
  }

  if (operationDone) {
    //enableInterrupt = false;
    // reset flag
    operationDone = false;
    if (transmitFlag) {
      // the previous operation was transmission, listen for response
      // print the result
      if (transmissionState == ERR_NONE) {
        // packet was successfully sent
        //Serial.println(F("transmission finished!"));
        blinkLed(2);

      } else {
        //Serial.print(F("failed, code "));
        //Serial.println(transmissionState);
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
          if (theData.typeId != 0) {
            String str = String(theData.typeId, DEC) + ",";
            str += String(theData.nodeId, DEC) + ",";
            str += String(theData.cmd, DEC) + ",";
            str += String(theData.vParams, DEC);  // 2,2,1,4$
            str += "$";
            Serial.print(str);
            blinkLed(2);
            if (gsmOnSta) {
              lastUpdateGSM = millis();
            } else {
              digitalWrite(GSM_ON, HIGH);  // nyalakan GSM
              gsmOnSta = true;
              lastUpdateGSM = millis();
            }
            //delay(100);
            //Serial.begin(115200);
            lastUpdate = millis();
          }
        }
      }
      radio.startReceive();
      transmitFlag = false;
    }
  }

  if (millis() - lastUpdate > 30000) {
    lastUpdate = millis();
    int vbat = analogRead(VBAT)*1.462;
    String str1 = String(DEVID) + "," + String(countUpdate) + "," + String(vbat) + "$";
    countUpdate++;
    Serial.print(str1);
    delay(10);
    // Serial.begin(115200);
  }
}
