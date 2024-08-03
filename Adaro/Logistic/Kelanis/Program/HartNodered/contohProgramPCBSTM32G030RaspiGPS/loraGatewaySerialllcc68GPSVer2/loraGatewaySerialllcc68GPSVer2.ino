#include <RadioLib.h>
#include <IWatchdog.h>
#include <EEPROM.h>
#include <CayenneLPP.h>
#include <TinyGPSPlus.h>

// Board GPSTrackerRaspiLora
#define LED PC15
#define NSS PA4
#define DIO1 PD0
#define NRST PD2
#define BUSY PD1

LLCC68 radio = new Module(NSS, DIO1, NRST, BUSY);
TinyGPSPlus gps;
static const int RXPin = PA3, TXPin = PA2;
HardwareSerial ss(RXPin, TXPin);

static const int keepAlive = 120000;
static const int updateGPSrate = 60000;

long lastUpdateGPS;
String DEVID = "GPS001";
bool VSRC;
bool transmitDone = false;
long lastUpdate;
bool updateSta = false;
int countUpdate = 0;

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
int transmissionState = RADIOLIB_ERR_NONE;

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

void sendDataLora(String str) {

  transmissionState = radio.startTransmit(str.c_str());
  transmitFlag = true;
  blinkLed(2);
  Serial.begin(115200);
}

void serial_flush() {
  while (true) {
    delay(20);  // give data a chance to arrive
    if (Serial.available()) {
      // we received something, get all of it and discard it
      while (Serial.available())
        Serial.read();
      continue;  // stay in the main loop
    } else
      break;  // nothing arrived for 20 ms
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  ss.begin(9600);
  pinMode(LED, OUTPUT);

  if (IWatchdog.isReset(true)) {
    // LED blinks to indicate reset
    blinkLed(10);
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

  analogReadResolution(12);

  // Init the watchdog timer with 10 seconds timeout
  IWatchdog.begin(10000000);
  lastUpdate = millis();
  lastUpdateGPS = lastUpdate;
  updateSta = false;
  blinkLed(3);
}

void loop() {
  // put your main code here, to run repeatedly:
  IWatchdog.reload();

  if (operationDone) {
    //enableInterrupt = false;
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

      // listen for response
      radio.startReceive();
      transmitFlag = false;
    } else {
      DynamicJsonDocument jsonBuffer(4096);
      CayenneLPP lpp(60);
      JsonArray root = jsonBuffer.to<JsonArray>();

      int state;
      if (state == RADIOLIB_ERR_NONE) {
        lpp.reset();
        // str += "$";
        size_t len = radio.getPacketLength();
        state = radio.readData(lpp.getBuffer(), len);
        lpp.decode(lpp.getBuffer(), len, root);
        serializeJsonPretty(root, Serial);
        // String str;
        // serializeJson(root, str);
        Serial.println();
        blinkLed(2);
        Serial.begin(115200);
      }
      radio.startReceive();
      transmitFlag = false;
    }
  }

  // if (millis() - lastUpdate > keepAlive) {
  //   lastUpdate = millis();
  //   String str1 = String(DEVID) + "," + String(countUpdate);
  //   countUpdate++;
  //   Serial.println(str1);
  //   delay(10);
  //   Serial.begin(115200);
  // }

  if (millis() - lastUpdateGPS > updateGPSrate) {
    lastUpdateGPS = millis();
    if (gps.location.isValid()) {
      String str = "GPS,";
      str += String(gps.location.lat(), 6) + ",";
      str += String(gps.location.lng(), 6) + ",";
      str += String(gps.altitude.meters(), 2) + ",";
      str += String(gps.course.deg(), 2) + ",";
      str += String(gps.speed.kmph(), 2);
      Serial.println(str);
    }
  }

  smartDelay(10);
}

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms) {
  unsigned long start = millis();
  do {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}