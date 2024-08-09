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
//

SX1276 lora = new Module(NSS, DIO0, LR_RST, DIO1);
//
const String DEVID = "NT001";

bool kirim = false;
const int waktuTunggu = 60000;
long startKirim;
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

  int state = lora.scanChannel();
  int cacah = 0;
  // tunggu sampai channel nya free
  while ((state == PREAMBLE_DETECTED) && (cacah < 5)) {
    delay(random(100, 2000));
    state = lora.scanChannel();
    cacah++;
  }
  state = lora.transmit(str.c_str());

  if (state == ERR_NONE) {
    // the packet was successfully transmitted
    blinkLed(2);
  }

  transmitDone = false;
  state = lora.startReceive();

}

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(LORA_ON, OUTPUT);
  digitalWrite(LORA_ON, LOW);

  if (IWatchdog.isReset(true)) {
    // LED blinks to indicate reset
    blinkLed(10);
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

//  // set the function that will be called
//  // when new packet is received
  lora.setDio0Action(setFlag);
  state = lora.startReceive();
   
  kirim = false;
}

void loop() {

  if (!kirim) {
    kirim = true;
    String str = String(DEVID) + ",1";
    String Checksum = xorChecksum(str);
    str += ":" + Checksum + "$";
    sendDataLora(str);
    //Serial.println(str);
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
      pos = str.indexOf(":");
      str = str.substring(0, pos);
      dataValid = true;
    }
    if (state == ERR_NONE) {
      if (dataValid) {
        // packet was successfully received

        String devid = getValue(str, ',', 0);
        Serial.println(devid);
        if (devid == DEVID) {
          String cmd = getValue(str, ',', 1);  // DEVID,1
          Serial.print("cmd ");
          Serial.println(cmd);
          if (cmd == "1") {       // DEVID,1
            blinkLed(5);
            
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
 
}
