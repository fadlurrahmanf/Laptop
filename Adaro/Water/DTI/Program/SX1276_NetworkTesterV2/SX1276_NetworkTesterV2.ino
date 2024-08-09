#include <RadioLib.h>
#include "STM32LowPower.h"

#define NSS     PA15
#define DIO0    PC2
#define LR_RST  PC10
#define DIO1    PC1
#define LED     PC13
#define LEDOUT  PA11
#define SENSOR  PB8
#define WAKE_UP PA0
#define ZENER   PA1
#define VCC     PA2
#define DONE    PB13


// include the library

//
//// SX1276 has the following connections:
//// NSS pin:   10
//// DIO0 pin:  2
//// RESET pin: 9
//// DIO1 pin:  3
SX1276 lora = new Module(NSS, DIO0, LR_RST, DIO1);
//
const String DEVID = "NT001";

bool kirim = false;
const int waktuTunggu = 60000;
long startKirim;


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

void setFlag(void) {
  // check if the interrupt is enabled
  if (!enableInterrupt) {
    return;
  }

  // we got a packet, set the flag
  receivedFlag = true;
}

void blinkLed(int n) {
  for (int i = 0; i < n; i++) {
    digitalWrite(LED, LOW);
    digitalWrite(LEDOUT, HIGH);
    delay(200);
    digitalWrite(LED, HIGH);
    digitalWrite(LEDOUT, LOW);
    delay(200);
  }
}

void sendSta(String str) {
  str += ":" + xorChecksum(str);
  str += "$";
  int state = lora.scanChannel();
  int cacah = 0;
  // tunggu sampai channel nya free
  while ((state == PREAMBLE_DETECTED) && (cacah < 10)) {
    //delay(random(1000, 60000));
    delay(300);
    state = lora.scanChannel();
    cacah++;
  }
  state = lora.transmit(str.c_str());

  if (state == ERR_NONE) {
    // the packet was successfully transmitted
    blinkLed(1);
  }
  state = lora.startReceive();
}

volatile int repetitions = 1;
void repetitionsIncrease() {
  // This function will be called once on device wakeup
  // You can do some little operations here (like changing variables which will be used in the loop)
  // Remember to avoid calling delay() and long running functions since this functions executes in interrupt context
  repetitions ++;
}

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(LEDOUT, OUTPUT);
  digitalWrite(LED, LOW);
  digitalWrite(LEDOUT, HIGH);


  // initialize SX1278 with default settings
  Serial.print(F("[SX1276] Initializing ... "));
  // carrier frequency:           915.0 MHz
  // bandwidth:                   125.0 kHz
  // spreading factor:            9
  // coding rate:                 7
  // sync word:                   0x12
  // output power:                17 dBm
  // current limit:               100 mA
  // preamble length:             8 symbols
  // amplifier gain:              0 (automatic gain control)
  //int state = lora.begin();
  int16_t begin(float freq = 434.0, float bw = 125.0, uint8_t sf = 9, uint8_t cr = 7, uint8_t syncWord = SX127X_SYNC_WORD, int8_t power = 17, uint8_t currentLimit = 100, uint16_t preambleLength = 8, uint8_t gain = 0);
  int state = lora.begin(915.0, 125.0, 9, 7, SX127X_SYNC_WORD, 17, 8, 0);
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }

  //  // set the function that will be called
  //  // when new packet is received
  lora.setDio0Action(setFlag);
  state = lora.startReceive();

  pinMode(WAKE_UP, INPUT_PULLUP);

//  LowPower.begin();
  // Attach a wakeup interrupt on pin, calling repetitionsIncrease when the device is woken up
//  LowPower.attachInterruptWakeup(WAKE_UP, repetitionsIncrease, RISING, SHUTDOWN_MODE);

  kirim = false;
}

void loop() {

  if (!kirim) {
    kirim = true;
    String str = String(DEVID) + ",1";
    sendSta(str);
    startKirim = millis();

  }

  if (receivedFlag) {
    // disable the interrupt service routine while
    // processing the data
    enableInterrupt = false;

    // reset flag
    receivedFlag = false;

    // you can read received data as an Arduino String
    String str;
    int state = lora.readData(str);
    Serial.println(str);
    bool dataValid = false;  // data from the server
    int pos = str.indexOf("#");
    if (pos > 5) {
      str = str.substring(0, pos);
      dataValid = true;
    }
    if (state == ERR_NONE) {
      if (dataValid) {
        // packet was successfully received

        String devid = getValue(str, ',', 0);
        String payload = getValue(str, ':', 0);
        String checkSum = getValue(str, ':', 1);
        String calcChecksum = xorChecksum(payload);
        if (checkSum == calcChecksum) {
          if (devid == DEVID) {
            String cmd = getValue(str, ',', 1);  // DEVID,1
            if (cmd == "1") {       // DEVID,1
              blinkLed(5);
//              LowPower.shutdown();
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

  if (millis() - startKirim > waktuTunggu) {
    digitalWrite(LEDOUT, LOW);
//    LowPower.shutdown();
  }
}
