#include <RadioLib.h>
#include <EEPROM.h>
#include "STM32LowPower.h"
#include <Wire.h>
//#include <INA219_WE.h>
#include <INA226_WE.h>
#define I2C_ADDRESS 0x40

#define HAL_PWR_MODULE_ENABLED

#define LED         PC13
#define VCCINA      PB9
#define GNDINA      PB8
#define SENSOREN    PC14
// board G030C8T6 RFM95
#define LR_RST      PA3
#define NSS         PA4
#define DIO0        PB1
#define DIO1        PA15
#define DIO2        PB3
#define LORA_PWR    PA11


#define TYPEID        5   // INA219
#define CTRBYTE     0x51

//INA219 ina(3.6f, 0.02f, 0x40);
// INA219_WE ina219 = INA219_WE(I2C_ADDRESS);
INA226_WE ina226 = INA226_WE(I2C_ADDRESS);
SX1276 radio = new Module(NSS, DIO0, LR_RST, DIO1);

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
} Params;
Params  id;

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
  byte rtn = 0;;
  for (byte i = 0; i < s; i++) {
    rtn ^= ar[i];
  }
  return rtn;
}

void sendStatus() {
  theData.typeId = id.typeId;
  theData.nodeId = id.nodeId;
  theData.cmd = 10;
  float current_mA = 0.0;
  // ina219.startSingleMeasurement();
  ina226.startSingleMeasurement();
  ina226.readAndClearFlags();
  current_mA = ina226.getCurrent_mA() * 100;
  // digitalWrite(SENSOREN, LOW);
  // current_mA = current_mA*100 - 20;
  theData.vParams = current_mA;
  byte len = sizeof(theData);
  byte byteArray[len + 1];
  memcpy(byteArray, (const void*)(&theData), len);
  byteArray[len] = calculateCRC(byteArray, len);
  int state = radio.transmit(byteArray, len + 1);
  if (state ==  RADIOLIB_ERR_NONE) {
    blinkLed(2);
  }
}

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(VCCINA, OUTPUT);
  pinMode(GNDINA, OUTPUT);
  pinMode(SENSOREN, OUTPUT);
  digitalWrite(LED, HIGH);
  digitalWrite(GNDINA, LOW);
  pinMode(LORA_PWR, OUTPUT);

  digitalWrite(SENSOREN, HIGH);
  digitalWrite(VCCINA, HIGH);
  digitalWrite(LORA_PWR, LOW);

  enableBackupDomain();
  
  EEPROM.get(0, id);
  if (id.configId == CTRBYTE) {
    
  }
  else {
    id.configId = CTRBYTE;
    id.typeId = TYPEID;
    id.nodeId = 12;
    id.modeSta = 1;
    id.periode = 30;
    EEPROM.put(0, id);
  }

  int state = radio.begin(920.0, 125.0, 9, 7,  RADIOLIB_SX127X_SYNC_WORD, 17, 8, 0);
  if (state ==  RADIOLIB_ERR_NONE) {
    //Serial.println(F("success!"));
  } else {
    //Serial.print(F("failed, code "));
    //Serial.println(state);
    blinkLed(20);
    while (true);
  }
  
  Wire.begin();
  if (!ina226.init()) {
    Serial.println("Failed to init INA226. Check your wiring.");
    blinkLed(10);
    while (1) {
    }
  }
  ina226.setAverage(AVERAGE_64);
  ina226.setMeasureMode(TRIGGERED);
  ina226.setResistorRange(3.6, 0.02);
  blinkLed(2);
  LowPower.begin();
}

void loop() {
  // digitalWrite(SENSOREN, HIGH);
  // digitalWrite(VCCINA, HIGH);
  // digitalWrite(LORA_PWR, LOW);
  delay(5000); // durasi nyala 1 menit
  sendStatus(); // mengirim data
  
  // delay(1000);
  // digitalWrite(SENSOREN, LOW);
  // digitalWrite(VCCINA, LOW);
  // digitalWrite(LORA_PWR, HIGH);
  // int waktuTidur = 10000 + random(1000); //durasi sleep 5 menit
  // delay(waktuTidur);
  // LowPower.shutdown(waktuTidur);
}
