#include <RadioLib.h>
#include <IWatchdog.h>
#include <EEPROM.h>

#define CTRBYTE       42

// Board wemosGatewayLoraLLCC68
#define LED       PC13
#define NSS       PA4
#define DIO1      PD0
#define NRST      PD2
#define BUSY      PD1

LLCC68 radio = new Module(NSS, DIO1, NRST, BUSY);

#define SERIAL_BAUD   115200

typedef struct {
  uint8_t       typeId;
  uint16_t      nodeId; //store this nodeId
  uint8_t       cmd; //type parameter
  uint32_t      vParams;   //temperature maybe?
} Payload;
Payload theData;

typedef struct {
  uint8_t configid;
  uint8_t devid;
} Params;
Params  id;

String DEVID = "GW94";
long lastUpdate;
int updateRate = 60000; // update rate 1 menit
int countUpdate = 0;

void blinkLed(int n) {
  for (int i = 0; i < n; i++) {
    digitalWrite(LED, LOW);
    delay(200);
    digitalWrite(LED, HIGH);
    delay(200);
  }
}

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

//Function to calculate CRC over an array 'ar' up to element 's'
byte calculateCRC(byte ar[], byte s) {
  byte rtn = 0;;
  for (byte i = 0; i < s; i++) {
    rtn ^= ar[i];
  }
  return rtn;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(SERIAL_BAUD);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  //#ifdef LORA_PWR
  // pinMode(LORA_PWR, OUTPUT);
  // digitalWrite(LORA_PWR, LOW);
  //#endif
  delay(10);

  if (IWatchdog.isReset(true)) {
    // LED blinks to indicate reset
    blinkLed(10);
  }

  EEPROM.get(0, id);
  if (id.configid == CTRBYTE) {
    DEVID = "GW" + String(id.devid);
  } else {
    id.configid = CTRBYTE;
    id.devid = 65;
    DEVID = "GW65";
    EEPROM.put(0, id);
  }

  int state = radio.begin(922.0, 125.0, 9, 7, SX127X_SYNC_WORD, 17, 8, 0);
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
  radio.setDio1Action(setFlag);

  state = radio.startReceive();
  if (state == ERR_NONE) {
    // Serial.println(F("success!"));
  } else {
    //Serial.print(F("failed, code "));
    //Serial.println(state);
    while (true);
  }

  // Init the watchdog timer with 10 seconds timeout
  IWatchdog.begin(10000000);
  lastUpdate = millis();
  blinkLed(3);
}

void loop() {
  // put your main code here, to run repeatedly:
  IWatchdog.reload();

  if (Serial.available() > 0) {
    String str = Serial.readStringUntil('#');  //2,2,3,445#ln
    //    Serial.begin(115200);
    Serial.flush();
    String tmp = getValue(str, ',', 0);
    if (tmp == "GW") {
      String dvid = getValue(str, ',', 1);
      DEVID = "GW" + dvid;
      id.devid = dvid.toInt();
      EEPROM.put(0, id);
    } else {

      transmissionState = radio.startTransmit(str);
      transmitFlag = true;

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
      String str = "";
      int state = radio.readData(str);
      if (state == ERR_NONE) {
        if (true) {
          if (true) { //theData.typeid
            //            String str = String(theData.typeId, DEC) + ",";
            //            str += String(theData.nodeId, DEC) + ",";
            //            str += String(theData.cmd, DEC) + ",";
            //            str += String(theData.vParams, DEC); // 2,2,1,4$
            str += "$";
            Serial.print(str);
            blinkLed(2);
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

  long wkt = millis();
  if ((wkt - lastUpdate) > updateRate) {
    lastUpdate = wkt;
    String str1 = String(DEVID) + "," + String(countUpdate) + "$";
    Serial.print(str1);
    //delay(10);
    //Serial.begin(115200);
    countUpdate++;
  }
}
