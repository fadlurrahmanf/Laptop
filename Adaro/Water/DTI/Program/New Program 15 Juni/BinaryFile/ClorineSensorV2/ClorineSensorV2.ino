// board STM32L051C8T6Mod

#include <RadioLib.h>
#include <IWatchdog.h>
#include <EEPROM.h>

#define LED        PC13
#define LR_RST     PB14
#define NSS        PA4
#define DIO0       PB1
#define DIO1       PA15
#define DIO2       PB3
#define LORA_ON    PA11   // active LOW

SX1276 lora = new Module(NSS, DIO0, LR_RST, DIO1);

const byte getMeasValue[] = {0x06, 0x03, 0x00, 0x00, 0x00, 0x04, 0x45, 0xBE};


String DEVID = "CLR100";
uint32_t updateRate = 120000;
long lastUpdate;
int clr, Temp;
int decCLR;
String msgSta;

//eeprom address
const uint32_t config_addr = 0;
const uint32_t devid_addr = 1;
const uint32_t mode_addr = 2;
const uint32_t periode_add = 3;

bool modeSta = true;
bool transmitDone = false;

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



String getValue(String data, char separator, int index)
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

// flag to indicate that a packet was received
volatile bool receivedFlag = false;

// disable interrupt when it's not needed
volatile bool enableInterrupt = true;

// this function is called when a complete packet
// is received by the module
// IMPORTANT: this function MUST be 'void' type
//            and MUST NOT have any arguments!
void setFlag(void) {
  // check if the interrupt is enabled
  if (!enableInterrupt) {
    return;
  }

  // we got a packet, set the flag
  receivedFlag = true;
}

void sendDataLora(String str) {

  String str1 = xorChecksum(str);

  str += ":" + str1 + "$";

  int state = lora.scanChannel();
  int cacah = 0;
  // tunggu sampai channel nya free
  while ((state == PREAMBLE_DETECTED) && (cacah < 5)) {
    delay(random(500, 4000));
    state = lora.scanChannel();
    cacah++;
  }
  state = lora.transmit(str.c_str());

  if (state == ERR_NONE) {
    // the packet was successfully transmitted
    blinkLed(2);
  }

  state = lora.startReceive();

}

bool makeMeasurement() {
  for (byte i = 0; i < 8; i++) {
    Serial.write(getMeasValue[i]);
  }
  delay(100);

  byte buf[15];

  if (Serial.available() > 0) {
    for (int i = 0; i < 13; i++) {
      buf[i] = Serial.read();
      delay(1);
    }
  }

  if ((buf[0] == 0x06) && (buf[1] == 0x03)) {
    int rawCLR = buf[3] * 256 + buf[4];
    decCLR = buf[5] * 256 + buf[6];
    int rawT = buf[7] * 256 + buf[8];
    int decT = buf[9] * 256 + buf[10];
    clr = rawCLR;
    Temp = rawT;
    return true;
  } else {
    clr = -1;
    Temp = -1;
    return false;
  }

}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(LORA_ON, OUTPUT);
  digitalWrite(LORA_ON, LOW);

  if (IWatchdog.isReset(true)) {
    // LED blinks to indicate reset
    blinkLed(10);
  }

  uint8_t config_code = EEPROM.read(config_addr);
  if (config_code == 40) {
    uint8_t devid_code = EEPROM.read(devid_addr);
    uint8_t mode_code = EEPROM.read(mode_addr);
    uint8_t periode_code = EEPROM.read(periode_add);
    DEVID = "CLR" + String(devid_code);
    if (mode_code == 0) {     // manual
      modeSta = false;
    } else {
      modeSta = true;
    }
    updateRate = periode_code * 1000;
  } else {
    EEPROM.write(config_addr, 40);  // config code
    EEPROM.write(devid_addr, 100);  // default devid address
    EEPROM.write(mode_addr, 1);   // manual transmit
    EEPROM.write(periode_add, 120);
  }

  int state = lora.begin(915.0, 125.0, 9, 7, SX127X_SYNC_WORD, 17, 8, 0);
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
  lora.setDio0Action(setFlag);

  state = lora.startReceive();

  //analogReadResolution(12);
  lastUpdate = millis();
  transmitDone = false;

  // Init the watchdog timer with 10 seconds timeout
  IWatchdog.begin(10000000);

  blinkLed(3);
}

void loop() {
  // put your main code here, to run repeatedly:
  IWatchdog.reload();
  
  if (!modeSta) {
    long now = millis();
    if ((now - lastUpdate) > updateRate) {
      lastUpdate = now + random(6000);
      String msg = "";
      bool sta = makeMeasurement();
      if (sta) {
        msg = String(DEVID) + "," + String(clr) + "," + String(decCLR) + "," + String(Temp);
        sendDataLora(msg);
      } else {
        msg = String(DEVID) + ",error";
        sendDataLora(msg);
        delay(1000);
        NVIC_SystemReset();
      }
    }
  }

  // check if the flag is set
  if (receivedFlag) {
    // disable the interrupt service routine while
    // processing the data
    enableInterrupt = false;

    // reset flag
    receivedFlag = false;

    if (transmitDone) {
      transmitDone = false;
    } else {
      // you can read received data as an Arduino String
      String str = "";
      int state = lora.readData(str);
      Serial.println(str);
      bool dataValid = false;  // data from the server
      int pos = str.indexOf("#");
      if (pos > 4) {
        str = str.substring(0, pos);
        String payload = getValue(str, ':', 0);
        String checkSum = getValue(str, ':', 1);
        String calcChecksum = xorChecksum(payload);
        if (checkSum == calcChecksum) {
          dataValid = true;
          str = payload;
        }
      }

      if (state == ERR_NONE) {
        // packet was successfully received
        if (dataValid) {
          String devid = getValue(str, ',', 0);

          if (devid == DEVID) {
            String cmd = getValue(str, ',', 1);
            if (cmd == "0") {
              String msg = "";
              bool sta = makeMeasurement();
              if (sta) {
                msg = String(DEVID) + "," + String(clr) + "," + String(decCLR) + "," + String(Temp);
                delay(1000);
                sendDataLora(msg);
              } else {
                msg = String(DEVID) + ",error";
                delay(1000);
                sendDataLora(msg);
                delay(1000);
                NVIC_SystemReset();
              }
            }
            if (cmd == "1") {       //devid
              String cfg = getValue(str, ',', 2);
              DEVID = "CLR" + cfg;
              EEPROM.write(devid_addr, cfg.toInt());
            }
            if (cmd == "2") {      //mode
              String cfg = getValue(str, ',', 2);
              if (cfg == "0") {
                modeSta = false;
              } else {
                modeSta = true;
              }
              EEPROM.write(mode_addr, cfg.toInt());
            }
            if (cmd == "3") {      //mode
              String cfg = getValue(str, ',', 2);
              uint8_t periode = cfg.toInt();
              updateRate = periode * 1000;
              EEPROM.write(periode_add, periode);
            }
            blinkLed(3);
          }
        }
      }
    }
    // put module back to listen mode
    lora.startReceive();

    // we're ready to receive more packets,
    // enable interrupt service routine
    enableInterrupt = true;
  }
}
