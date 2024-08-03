#include <RadioLib.h>
#include <EEPROM.h>
#include "STM32LowPower.h"
#include <Wire.h>
#include <INA219_WE.h>
#define I2C_ADDRESS 0x40

#define HAL_PWR_MODULE_ENABLED

#define LED         PC13
#define VCCINA      PA10
#define GNDINA      PA9
// board loraSTM32L071ModuleV2
#define LR_RST      PC4
#define NSS         PA4
#define DIO0        PB5
#define DIO1        PB4
#define DIO2        PB3
#define LORA_PWR    PB8
#define SENSOREN    PB12

#define TYPEID        5   // INA219
#define CTRBYTE     0x54

//INA219 ina(3.6f, 0.02f, 0x40);
INA219_WE ina219 = INA219_WE(I2C_ADDRESS);
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
  ina219.startSingleMeasurement();
  current_mA = ina219.getCurrent_mA();
  current_mA = current_mA*100 - 20;
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
  digitalWrite(VCCINA, HIGH);
  digitalWrite(GNDINA, LOW);
  digitalWrite(SENSOREN, LOW);
  pinMode(LORA_PWR, OUTPUT);
  digitalWrite(LORA_PWR, LOW);

  //enableBackupDomain();
  
  EEPROM.get(0, id);
  if (id.configId == CTRBYTE) {
    
  }
  else {
    id.configId = CTRBYTE;
    id.typeId = TYPEID;
    id.nodeId = 4;
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
  
  if(!ina219.init()){
    blinkLed(10);
    //Serial.println("INA219 not connected!");
  }
  ina219.setADCMode(BIT_MODE_12);
  ina219.setMeasureMode(TRIGGERED);
  ina219.setPGain(PG_80);
  ina219.setBusRange(BRNG_32);
  ina219.setShuntSizeInOhms(3.6);
  LowPower.begin();
}

void loop() {
  delay(6000); // durasi nyala 1 menit
  sendStatus(); // mengirim data
  int waktuTidur = 120000 + random(1000); //durasi sleep 5 menit
  LowPower.shutdown(waktuTidur);
}
