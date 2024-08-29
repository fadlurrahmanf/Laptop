//  FARM DATA RELAY SYSTEM
//
//  ESP-NOW Sensor Example
//
//  Developed by Timm Bogner (timmbogner@gmail.com) in Urbana, Illinois, USA.
//  An example of how to send data via ESP-NOW using FDRS.
//
#include "fdrs_node_config.h"
#include <fdrs_node.h>
#include <ESP_EEPROM.h>
#include <SimpleSerialShell.h>

#define RLY_ON 4
#define RLY_OFF 5
#define CF_INPUT 12

uint8_t typeId = 6;

struct idStruc {
  uint8_t configId;
  uint8_t typeId;
  uint16_t nodeId;
  uint8_t gtwyId;
  uint8_t mode;
  uint8_t periode;
  uint16_t counter;
} id;

const uint16_t timeout = 1000;  //Serial timeout[ms]
const float Vref = 1.218;       //[V]
// const float RL = 0.5;           //[mOhm]
// const float R1 = 0.5230;        //[kOhm]
// const float R2 = 5 * 390.0;     //[kOhm]
const float R8 = 5 * 20;    //kOhm, Rsum
const float R7 = 0.0249;    //kOhm
const float R5 = 3.3;       // Ohm
const float Rt = 1000;      // CT spec
uint16_t Hz = 50;           //[Hz]
uint16_t updateRate = 400;  //[ms]

bool rlyStatus = true;
bool changeRly = true;

uint8_t updatePeriode = 120;
uint32_t updateRateSend = updatePeriode * 1000;
long lastUpdate;
bool sendSta = false;
bool needRst = false;
bool updateEEPROM = false;
uint16_t lastCounter;
uint16_t cacahs;



#define LED LED_BUILTIN

void blinkLed(int n) {
  for (int i = 0; i < n; i++) {
    digitalWrite(LED, HIGH);
    delay(200);
    digitalWrite(LED, LOW);
    delay(200);
  }
}

int showID(int /*argc*/ = 0, char ** /*argv*/ = NULL) {
  shell.println(F("Running " __FILE__ ", Built " __DATE__));
  return 0;
};

int getInfo(int /*argc*/ = 0, char ** /*argv*/ = NULL) {
  shell.println(F("Running " __FILE__ ", Built " __DATE__));
  shell.print("nodeId is ");
  shell.println(id.nodeId);
  shell.print("networkId is ");
  shell.println(id.gtwyId);
  return 0;
};

////////////////////////////////////////////////////////////////////////////////
int setNodeId(int argc, char **argv) {
  if (argc != 2) {
    shell.println("bad argument count");
    return -1;
  }
  id.nodeId = atoi(argv[1]);
  shell.print("Setting nodeId to ");
  shell.println(id.nodeId);
  EEPROM.put(0, id);
  EEPROM.commit();
  needRst = true;
  return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
int getNodeId(int /*argc*/, char ** /*argv*/) {
  shell.print("nodeId is ");
  shell.println(id.nodeId);
  return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
int setGatewayId(int argc, char **argv) {
  if (argc != 2) {
    shell.println("bad argument count");
    return -1;
  }
  id.gtwyId = atoi(argv[1]);
  shell.print("Setting networkId to ");
  shell.println(id.gtwyId);
  EEPROM.put(0, id);
  EEPROM.commit();
  needRst = true;
  return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
int getGatewayId(int /*argc*/, char ** /*argv*/) {
  shell.print("networkId is ");
  shell.println(id.gtwyId);
  return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
int setUpdatePeriode(int argc, char **argv) {
  if (argc != 2) {
    shell.println("bad argument count");
    return -1;
  }
  id.periode = atoi(argv[1]);
  shell.print("Setting updatePeriode to ");
  shell.println(id.periode);
  EEPROM.put(0, id);
  EEPROM.commit();
  updateRateSend = id.periode * 1000;
  return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
int getUpdatePeriode(int /*argc*/, char ** /*argv*/) {
  shell.print("updatePeriode is ");
  shell.println(id.periode);
  return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
int setMode(int argc, char **argv) {
  if (argc != 2) {
    shell.println("bad argument count");
    return -1;
  }
  id.mode = atoi(argv[1]);
  shell.print("Setting mode to ");
  shell.println(id.mode);
  EEPROM.put(0, id);
  EEPROM.commit();
  return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
int getMode(int /*argc*/, char ** /*argv*/) {
  shell.print("Mode is ");
  shell.println(id.mode);
  return EXIT_SUCCESS;
}

int setCounter(int argc, char **argv) {
  if (argc != 2) {
    shell.println("bad argument count");
    return -1;
  }
  id.counter = atoi(argv[1]);
  shell.print("Setting mode to ");
  shell.println(id.counter);
  updateEEPROM = true;
  return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
int getCounter(int /*argc*/, char ** /*argv*/) {
  shell.print("Mode is ");
  shell.println(id.counter);
  return EXIT_SUCCESS;
}




void dumpByteArray(const byte *byteArray, const byte arraySize) {
  for (int i = 0; i < arraySize; i++) {
    Serial.print("0x");
    if (byteArray[i] < 0x10)
      Serial.print("0");
    Serial.print(byteArray[i], HEX);
    Serial.print(", ");
  }
  Serial.println();
}

void fdrs_recv_cb(DataReading theData) {
  DBG("ID: " + String(theData.id));
  DBG("Type: " + String(theData.t));
  DBG("Data: " + String(theData.d));
  int var = theData.t;
  switch (var) {
    case 1:
      // send status
      sendSta = true;
      break;
    case 2:
      // change Relay
      if (theData.d > 0) {
        rlyStatus = true;
      } else {
        rlyStatus = false;
      }
      changeRly = true;
      break;
    case 3:
      // change nodeId
      id.nodeId = theData.d;
      EEPROM.put(0, id);
      EEPROM.commit();
      needRst = true;
      break;
    case 4:
      // change nodeId
      id.gtwyId = theData.d;
      EEPROM.put(0, id);
      EEPROM.commit();
      needRst = true;
      break;
    case 5:
      // change nodeId
      id.mode = theData.d;
      updateEEPROM = true;
      break;
    case 6:
      // change nodeId
      id.periode = theData.d;
      updateRateSend = id.periode * 1000;
      updateEEPROM = true;
      break;
    case 7:
      // change nodeId
      id.counter = theData.d;
      updateEEPROM = true;
      break;
    default:
      // if nothing else matches, do the default
      // default is optional
      break;
  }
}

void sendStatus() {

  Serial.begin(4800);
  delay(10);
  Serial.swap();
  Serial.setRxBufferSize(1024);
  delay(10);
  Reset();
  setFrequency(50);
  setUpdateRate(800);
  float voltage = -1;
  if (getVoltage(&voltage)) {
    loadFDRS(voltage, VOLTAGE_T);
  }
  float current = -1;
  if (getCurrent(&current)) {
    loadFDRS(current, CURRENT_T);
  }
  float activePower = -1;
  if (getActivePower(&activePower)) {
    loadFDRS(activePower, POWER_T);
  }
  float activeEnergy = -1;
  if (getActiveEnergy(&activeEnergy)) {
    loadFDRS(activeEnergy, ENERGY_T);
  }
  float powerFactor = -1;
  if (getPowerFactor(&powerFactor)) {
    loadFDRS(powerFactor, POWER2_T);
  }
  loadFDRS(id.counter, IT_T);

  Serial.swap();
  Serial.begin(115200);
  delay(10);
  Serial.print("voltage :");
  Serial.println(voltage);
  if (rlyStatus) {
    loadFDRS(1, STATUS_T);
  } else {
    loadFDRS(0, STATUS_T);
  }
  // Serial.end();
  // Serial.begin(115200);
  Serial.println("Kirim");
  DBG(sendFDRS());
  if (sendFDRS()) {
    DBG("Big Success!");
    Serial.println("OK");
  } else {
    DBG("Nope, not so much.");
    Serial.println("Not OK");
  }

  if (id.counter > lastCounter) {
    lastCounter = id.counter;
    updateEEPROM = true;
  }

  blinkLed(3);
}

void changeRelay(bool input) {
  if (input) {
    digitalWrite(RLY_ON, HIGH);
    delay(100);
    digitalWrite(RLY_ON, LOW);
  } else {
    digitalWrite(RLY_OFF, HIGH);
    delay(100);
    digitalWrite(RLY_OFF, LOW);
  }
}

void ICACHE_RAM_ATTR cfInput() {
  cacahs++;
  if(cacahs>=33333){
    cacahs = 0 ;
    id.counter++;
  }
}

void setup() {

  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  pinMode(RLY_ON, OUTPUT);
  pinMode(RLY_OFF, OUTPUT);
  digitalWrite(RLY_ON, LOW);
  digitalWrite(RLY_OFF, LOW);
  pinMode(CF_INPUT, INPUT);
  attachInterrupt(CF_INPUT, cfInput, FALLING);

  EEPROM.begin(sizeof(idStruc));
  EEPROM.get(0, id);
  if (id.configId == 0x22) {
    READING_ID = id.nodeId;
    GTWY_MAC = id.gtwyId;
    typeId = id.typeId;
    updateRateSend = id.periode * 1000;
  } else {
    id.configId = 0x22;
    id.nodeId = READING_ID;
    id.gtwyId = GTWY_MAC;
    id.typeId = typeId;
    id.mode = 1;
    id.periode = updatePeriode;
    id.counter = 0;
    updateRateSend = id.periode * 1000;
    EEPROM.put(0, id);
    bool ok = EEPROM.commit();
  }
  lastCounter = id.counter;
  beginFDRS();
  pingFDRS(1000);
  addFDRS(fdrs_recv_cb);
  subscribeFDRS(READING_ID);

  // SerialShell command
  shell.attach(Serial);
  shell.addCommand(F("id?"), showID);
  shell.addCommand(F("setGatewayId <gtwyId>"), setGatewayId);
  shell.addCommand(F("getGatewayId"), getGatewayId);
  shell.addCommand(F("setNodeId <nodeId>"), setNodeId);
  shell.addCommand(F("getNodeId"), getNodeId);
  shell.addCommand(F("setMode <mode>"), setMode);
  shell.addCommand(F("getMode"), getMode);
  shell.addCommand(F("setUpdatePeriode <updatePeriode>"), setUpdatePeriode);
  shell.addCommand(F("getUpdatePeriode"), getUpdatePeriode);
  shell.addCommand(F("setCounter <counter>"), setCounter);
  shell.addCommand(F("getCounter"), getCounter);
  Serial.println();
  
  lastUpdate = millis();
  delay(2000);
  getInfo();
  delay(2000);
  sendStatus();
}

void loop() {

  if (id.mode == 1) {
    if ((millis() - lastUpdate) >= (updateRateSend)) {
      uint8_t pn = random(2);
      uint16_t rnd = (random(5000));
      if (pn) {
        lastUpdate = millis() + rnd - 5000;
      } else {
        lastUpdate = millis() - rnd - 5000;
      }
      delay(1000);
      sendStatus();
      Serial.println("PN : "+String(pn));
      Serial.println("rnd : "+String(rnd));
      delay(1000);
    }
  }

  if (sendSta) {
    sendSta = false;
    sendStatus();
  }

  if (needRst) {
    ESP.restart();
  }

  if (changeRly) {
    changeRly = false;
    changeRelay(rlyStatus);
    // sendStatus();
  }

  if (updateEEPROM) {
    updateEEPROM = false;
    EEPROM.put(0, id);
    bool ok = EEPROM.commit();
  }

  loopFDRS();
  shell.executeIfInput();
}


String dumpByteArrayToString(const byte *byteArray, const byte arraySize) {
  String str = "";
  for (int i = 0; i < arraySize; i++) {
    if (byteArray[i] < 0x10)
      str += "0";
    str += String(byteArray[i], HEX);
  }
  return str;
}

uint8_t _culcCheckSum(uint8_t *txData, int txLenght, uint8_t *rxData, int rxLenght) {

  uint8_t checksum = 0;
  for (int i = 0; i < txLenght; i++) {
    checksum += txData[i];
  }
  for (int i = 0; i < rxLenght; i++) {
    checksum += rxData[i];
  }
  checksum = ~checksum;
  return checksum;
}

bool _writeRegister(uint8_t address, uint32_t data) {
  //read buffer clear
  while (Serial.available() != 0) {
    Serial.read();
  }

  //Register Unlock
  uint8_t unlockTxData[6] = { 0xA8, 0x1A, 0x55, 0, 0, 0 };
  unlockTxData[5] = _culcCheckSum(unlockTxData, sizeof(unlockTxData) - 1, 0, 0);
  Serial.write(unlockTxData, sizeof(unlockTxData));

  //Write Register
  uint8_t txData[6] = { 0xA8, address, (uint8_t)(data), (uint8_t)(data >> 8), (uint8_t)(data >> 16) };
  txData[5] = _culcCheckSum(txData, sizeof(txData) - 1, 0, 0);
  Serial.write(txData, sizeof(txData));

  return true;
}

bool _readRegister(uint8_t address, uint32_t *data) {
  uint8_t txData[] = { 0x58, address };
  Serial.write(txData, sizeof(txData));

  uint8_t rxData[4] = { 0, 0, 0, 0 };
  uint32_t startTime = millis();
  while (Serial.available() != sizeof(rxData)) {
    delay(10);
    if ((millis() - startTime) > timeout)
      break;
  }
  int rxDataLength = Serial.readBytes(rxData, sizeof(rxData));

  if (rxDataLength == 0) {
    // Serial.println("Serial Timeout.");
    return false;
  }

  uint8_t checksum = _culcCheckSum(txData, sizeof(txData), rxData, sizeof(rxData) - 1);
  if (rxData[3] != checksum) {
    char massage[128];
    sprintf(massage, "Checksum error truet:%x read:%x.", checksum, rxData[3]);
    //Serial.println(massage);
    return false;
  }

  *data = ((uint32_t)rxData[2] << 16) | ((uint32_t)rxData[1] << 8) | (uint32_t)rxData[0];
  return true;
}

// bool getCurrent(float *current) {
//   uint32_t data;
//   if (false == _readRegister(0x04, &data)) {
//     //Serial.println("Can not read I_RMS register.");
//     return false;
//   }
//   *current = (float)data * Vref / ((324004.0 * RL));
//   return true;
// }

bool getCurrent(float *current) {
  uint32_t data;
  if (false == _readRegister(0x04, &data)) {
    //Serial.println("Can not read I_RMS register.");
    return false;
  }
  *current = (float)data * Vref / ((324004.0 * (R5 * 1000) / Rt));
  return true;
}
// bool getVoltage(float *voltage) {
//   uint32_t data;
//   if (false == _readRegister(0x06, &data)) {
//     //Serial.println("Can not read V_RMS register.");
//     return false;
//   }

//   *voltage = (float)data * Vref * (R2 + R1) / (79931.0 * R1 * 1000);
//   return true;
// }
bool getVoltage(float *voltage) {
  uint32_t data;
  if (false == _readRegister(0x06, &data)) {
    //Serial.println("Can not read V_RMS register.");
    return false;
  }

  *voltage = (float)data * Vref * (R8) / (79931.0 * R7 * 1000);
  return true;
}

// bool getActivePower(float *activePower) {
//   uint32_t data;
//   if (false == _readRegister(0x08, &data)) {
//     //Serial.println("Can not read WATT register.");
//     return false;
//   }

//   int32_t rowActivePower = (int32_t)(data << 8) / 256;
//   if (rowActivePower < 0)
//     rowActivePower = -rowActivePower;
//   *activePower = (float)rowActivePower * Vref * Vref * (R2 + R1) / (4046.0 * RL * 1000.0 * R1);
//   return true;
// }
bool getActivePower(float *activePower) {
  uint32_t data;
  if (false == _readRegister(0x08, &data)) {
    //Serial.println("Can not read WATT register.");
    return false;
  }

  int32_t rowActivePower = (int32_t)(data << 8) / 256;
  if (rowActivePower < 0)
    rowActivePower = -rowActivePower;
  *activePower = (float)rowActivePower * Vref * Vref * (R8) / (4046.0 * ((R5 * 1000) / Rt) * 1000.0 * R7);
  return true;
}

// bool getActiveEnergy(float *activeEnergy) {

//   uint32_t data;
//   if (false == _readRegister(0x0A, &data)) {
//     //Serial.println("Can not read CF_CNT register.");
//     return false;
//   }

//   int32_t rowCF_CNT = (int32_t)(data << 8) / 256;
//   if (rowCF_CNT < 0)
//     rowCF_CNT = -rowCF_CNT;
//   //Serial.print("Float de Energia: ");
//   //Serial.println(rowCF_CNT);
//   *activeEnergy = (float)rowCF_CNT * 1638.4 * 256.0 * Vref * Vref * (R2 + R1) / (3600000.0 * 4046.0 * (RL * 1000.0) * R1);

//   return true;
// }

bool getActiveEnergy(float *activeEnergy) {

  uint32_t data;
  if (false == _readRegister(0x0A, &data)) {
    //Serial.println("Can not read CF_CNT register.");
    return false;
  }

  int32_t rowCF_CNT = (int32_t)(data << 8) / 256;
  if (rowCF_CNT < 0)
    rowCF_CNT = -rowCF_CNT;
  //Serial.print("Float de Energia: ");
  //Serial.println(rowCF_CNT);
  *activeEnergy = (float)rowCF_CNT * 1638.4 * 256.0 * Vref * Vref * (R8) / (3600000.0 * 4046.0 * ((R5 * 1000.0) / Rt) * R7 * 1000);

  return true;
}

bool getPowerFactor(float *powerFactor) {
  uint32_t data;
  if (false == _readRegister(0x0C, &data)) {
    //Serial.println("Can not read CORNER register.");
    return false;
  }

  float rowPowerFactor = cos(2.0 * 3.1415926535 * (float)data * (float)Hz / 1000000.0) * 100.0;
  if (rowPowerFactor < 0)
    rowPowerFactor = -rowPowerFactor;
  *powerFactor = rowPowerFactor;

  return true;
}

bool getTemperature(float *temperature) {
  uint32_t data;
  if (false == _readRegister(0x0E, &data)) {
    //Serial.println("Can not read TPS1 register.");
    return false;
  }

  int16_t rowTemperature = (int16_t)(data << 6) / 64;
  *temperature = (170.0 / 448.0) * (rowTemperature / 2.0 - 32.0) - 45;
  return true;
}

bool Reset() {
  if (false == _writeRegister(0x19, 0x5A5A5A)) {
    //Serial.println("Can not write SOFT_RESET register.");
    return false;
  }
  while (Serial.available() != 0) {
    Serial.read();
  }

  delay(500);
  return true;
}

bool setUpdateRate(uint32_t rate) {
  uint32_t data;
  if (false == _readRegister(0x18, &data)) {
    //Serial.println("Can not read MODE register.");
    return false;
  }

  uint16_t mask = 0b0000000100000000;  //8bit
  if (rate == 400)
    data &= ~mask;
  else
    data |= mask;

  if (false == _writeRegister(0x18, data)) {
    //Serial.println("Can not write MODE register.");
    return false;
  }

  if (false == _readRegister(0x18, &data)) {
    //Serial.println("Can not read MODE register.");
    return false;
  }

  if ((data & mask) == 0) {
    updateRate = 400;
    //Serial.println("Set update rate:400ms.");
  } else {
    updateRate = 800;
    //Serial.println("Set update rate:800ms.");
  }
  return true;
}

bool setFrequency(uint32_t Hz) {
  uint32_t data;
  if (false == _readRegister(0x18, &data)) {
    //Serial.println("Can not read MODE register.");
    return false;
  }

  uint16_t mask = 0b0000001000000000;  //9bit
  if (Hz == 50)
    data &= ~mask;
  else
    data |= mask;

  if (false == _writeRegister(0x18, data)) {
    //Serial.println("Can not write MODE register.");
    return false;
  }

  if (false == _readRegister(0x18, &data)) {
    //Serial.println("Can not read MODE register.");
    return false;
  }

  if ((data & mask) == 0) {
    Hz = 50;
    //Serial.println("Set frequency:50Hz");
  } else {
    Hz = 60;
    //Serial.println("Set frequency:60Hz");
  }
  return true;
}

// bool setOverCurrentDetection(float detectionCurrent) {
//   const float magicNumber = 0.72;  // I_FAST_RMS = 0.72 * I_RMS (Values obtained by experiments in the case of resistance load)

//   //MODE[12] CF_UNABLE set 1 : alarm, enable by TPS_CTRL[14] configured
//   uint32_t data;
//   if (false == _readRegister(0x18, &data)) {
//     //Serial.println("Can not read MODE register.");
//     return false;
//   }
//   data |= 0b0001000000000000;  //12bit
//   if (false == _writeRegister(0x18, data)) {
//     //Serial.println("Can not read write register.");
//     return false;
//   }

//   //TPS_CTRL[14] Alarm switch set 1 : Over-current and leakage alarm on
//   if (false == _readRegister(0x1B, &data)) {
//     //Serial.println("Can not read TPS_CTRL register.");
//     return false;
//   }
//   data |= 0b0100000000000000;  //14bit  0b0100000000000000
//   if (false == _writeRegister(0x1B, data)) {
//     //Serial.println("Can not write TPS_CTRL register.");
//     return false;
//   }

//   //Set detectionCurrent I_FAST_RMS_CTRL
//   data = (uint32_t)(detectionCurrent * magicNumber / Vref * ((324004.0 * RL * 1000.0)));
//   data >>= 9;
//   data &= 0x007FFF;
//   float actualDetectionCurrent = (float)(data << 9) * Vref / ((324004.0 * RL * 1000.0));
//   data |= 0b1000000000000000;  //15bit=1 Fast RMS refresh time is every cycle
//   data &= 0x00000000FFFFFFFF;
//   if (false == _writeRegister(0x10, data)) {
//     //Serial.println("Can not write I_FAST_RMS_CTRL register.");
//     return false;
//   }
//   char massage[128];
//   sprintf(massage, "Set Current Detection:%.1fA.", actualDetectionCurrent);
//   //Serial.println(massage);

//   return true;
// }

bool setCFOutputMode() {
  //MODE[12] CF_UNABLE set 0 : alarm, enable by TPS_CTRL[14] configured
  uint32_t data;
  if (false == _readRegister(0x18, &data)) {
    //Serial.println("Can not read MODE register.");
    return false;
  }
  data &= ~0b0001000000000000;  //12bit
  if (false == _writeRegister(0x18, data)) {
    //Serial.println("Can not read write register.");
    return false;
  }
  return true;
}
