// Define User Types below here or use a .h file
#include <RadioLib.h>
#include <IWatchdog.h>
#include <EEPROM.h>
#include <CayenneLPP.h>
#include <DFRobot_LWLP.h>
#include <SimpleSerialShell.h>

// Board PCB_STM32G030C8T6
#define LED PC13
#define NSS PA4
#define DIO0 PB1
#define DIO1 PA15
#define NRST PA3
#define LORA_ON PA11

#define CTRBYTE 0x03

#define STATUSPIN PB10
#define READSTATUS PB11

uint8_t updatePeriode = 23;
int updateRateSend = updatePeriode * 1000;
long lastUpdate;
bool sendSta = false;
uint8_t mode = 0;
uint8_t deviceId = 3;
uint8_t networkId = 2;
uint8_t typeId = 2; // Pressure
float temperature, pressure;

SX1276 radio = new Module(NSS, DIO0, NRST, DIO1);
DFRobot_LWLP lwlp;

// save transmission states between loops
int transmissionState = RADIOLIB_ERR_NONE;

// flag to indicate transmission or reception state
bool transmitFlag = false;

// flag to indicate that a packet was sent or received
volatile bool operationDone = false;

// this function is called when a complete packet
// is transmitted or received by the module
// IMPORTANT: this function MUST be 'void' type
//            and MUST NOT have any arguments!
void setFlag(void)
{
  // we sent or received a packet, set the flag
  operationDone = true;
}

void blinkLed(int n)
{
  for (int i = 0; i < n; i++)
  {
    digitalWrite(LED, LOW);
    delay(200);
    digitalWrite(LED, HIGH);
    delay(200);
  }
}

typedef struct Payload
{
  uint8_t networkId;
  uint8_t deviceId;
  uint8_t cmd;      // type parameter
  uint32_t vParams; // temperature maybe?
};
Payload theData;

typedef struct
{
  uint8_t configId; // control word
  uint8_t typeId;   // type Id : jenis sensor/actuator
  uint8_t networkId;
  uint8_t deviceId;
  uint8_t mode;    // autoUpdate : 0=manual, 1=auto
  uint8_t periode; // update rate in second for autoupdate
} Params;
Params id;

// Function to calculate CRC over an array 'ar' up to element 's'
byte calculateCRC(byte ar[], byte s)
{
  byte rtn = 0;
  ;
  for (byte i = 0; i < s; i++)
  {
    rtn ^= ar[i];
  }
  return rtn;
}

void sendStatus()
{
  uint8_t bufArray[60];
  uint8_t cursor = 0;
  bufArray[cursor] = networkId;
  cursor++;
  bufArray[cursor] = deviceId;
  cursor++;
  bufArray[cursor] = typeId;
  cursor++;
  CayenneLPP lpp(60);
  lpp.reset();
  lpp.addTemperature(1, temperature);
  lpp.addBarometricPressure(1, pressure);
  uint8_t len = lpp.getSize();
  bufArray[cursor] = len;
  cursor++;
  uint8_t bufData[50];
  lpp.copy(bufData);
  for (uint8_t i = 0; i < len; i++)
  {
    bufArray[cursor] = bufData[i];
    cursor++;
  }
  uint8_t crcData = calculateCRC(bufArray, cursor);
  bufArray[cursor] = crcData;
  cursor++;
  transmissionState = radio.startTransmit(bufArray, cursor);
  transmitFlag = true;
  // blinkLed(2);
}

int showID(int /*argc*/ = 0, char ** /*argv*/ = NULL)
{
  shell.println(F("Running " __FILE__ ", Built " __DATE__));
  return 0;
};

////////////////////////////////////////////////////////////////////////////////
int setDeviceId(int argc, char **argv)
{
  if (argc != 2)
  {
    shell.println("bad argument count");
    return -1;
  }
  deviceId = atoi(argv[1]);
  shell.print("Setting deviceId to ");
  shell.println(deviceId);
  id.deviceId = deviceId;
  EEPROM.put(0, id);
  return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
int getDeviceId(int /*argc*/, char ** /*argv*/)
{
  shell.print("deviceId is ");
  shell.println(deviceId);
  return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
int setNetworkId(int argc, char **argv)
{
  if (argc != 2)
  {
    shell.println("bad argument count");
    return -1;
  }
  networkId = atoi(argv[1]);
  shell.print("Setting networkId to ");
  shell.println(networkId);
  id.networkId = networkId;
  EEPROM.put(0, id);
  return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
int getNetworkId(int /*argc*/, char ** /*argv*/)
{
  shell.print("networkId is ");
  shell.println(networkId);
  return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
int setUpdatePeriode(int argc, char **argv)
{
  if (argc != 2)
  {
    shell.println("bad argument count");
    return -1;
  }
  updatePeriode = atoi(argv[1]);
  shell.print("Setting updatePeriode to ");
  shell.println(updatePeriode);
  id.periode = updatePeriode;
  EEPROM.put(0, id);
  updateRateSend = updatePeriode * 1000;
  return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
int getUpdatePeriode(int /*argc*/, char ** /*argv*/)
{
  shell.print("updatePeriode is ");
  shell.println(updatePeriode);
  return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
int setMode(int argc, char **argv)
{
  if (argc != 2)
  {
    shell.println("bad argument count");
    return -1;
  }
  mode = atoi(argv[1]);
  shell.print("Setting mode to ");
  shell.println(mode);
  id.mode = mode;
  EEPROM.put(0, id);
  return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
int getMode(int /*argc*/, char ** /*argv*/)
{
  shell.print("Mode is ");
  shell.println(mode);
  return EXIT_SUCCESS;
}

int generateRandom(int minValue, int maxValue)
{
  static unsigned long seed = 0;
  seed ^= millis(); // Menggunakan waktu milidetik sebagai entropi

  // XOR shift algoritma sederhana untuk generator pseudo-acak
  seed ^= seed << 13;
  seed ^= seed >> 17;
  seed ^= seed << 5;

  // Menghasilkan nilai acak dalam rentang yang diinginkan
  int range = maxValue - minValue;
  int randNumber = minValue + (seed % range);

  return randNumber;
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  pinMode(LORA_ON, OUTPUT);
  digitalWrite(LORA_ON, LOW);

  if (IWatchdog.isReset(true))
  {
    // LED blinks to indicate reset
    blinkLed(10);
  }

  while (lwlp.begin() != 0)
  {
    Serial.println("Failed to initialize the chip, please confirm the chip connection");
    delay(1000);
  }

  EEPROM.get(0, id);
  if (id.configId == CTRBYTE)
  {
    typeId = id.typeId;
    networkId = id.networkId;
    deviceId = id.deviceId;
    updatePeriode = id.periode;
    mode = id.mode;
    updateRateSend = 1000 * id.periode;
  }
  else
  {
    id.configId = CTRBYTE;
    id.typeId = typeId;
    id.deviceId = deviceId;
    id.networkId = networkId;
    id.mode = 1;
    id.periode = updatePeriode;
    EEPROM.put(0, id);
  }

  int state = radio.begin(921.0, 125.0, 9, 7, RADIOLIB_SX127X_SYNC_WORD, 17, 8, 0);
  if (state == RADIOLIB_ERR_NONE)
  {
    // Serial.println(F("success!"));
  }
  else
  {
    // Serial.print(F("failed, code "));
    // Serial.println(state);
    blinkLed(20);
    while (true)
      ;
  }

  // set the function that will be called
  // when new packet is received
  radio.setDio0Action(setFlag, RISING);

  state = radio.startReceive();
  if (state == RADIOLIB_ERR_NONE)
  {
    // Serial.println(F("success!"));
  }
  else
  {
    // Serial.print(F("failed, code "));
    // Serial.println(state);
    while (true)
      ;
  }

  // Init the watchdog timer with 10 seconds timeout
  IWatchdog.begin(10000000);
  lastUpdate = millis();
  sendSta = false;
  blinkLed(3);

  pinMode(STATUSPIN, OUTPUT);
  digitalWrite(STATUSPIN, HIGH);
  pinMode(READSTATUS, INPUT);

  shell.attach(Serial);
  shell.addCommand(F("id?"), showID);
  shell.addCommand(F("setNetworkId <networkId>"), setNetworkId);
  shell.addCommand(F("getNetworkId"), getNetworkId);
  shell.addCommand(F("setDeviceId <deviceId>"), setDeviceId);
  shell.addCommand(F("getDeviceId"), getDeviceId);
  shell.addCommand(F("setMode <mode>"), setMode);
  shell.addCommand(F("getMode"), getMode);
  shell.addCommand(F("setUpdatePeriode <updatePeriode>"), setUpdatePeriode);
  shell.addCommand(F("getUpdatePeriode"), getUpdatePeriode);
  showID();
}

void loop()
{
  // put your main code here, to run repeatedly:
  IWatchdog.reload();

  DFRobot_LWLP::sLwlp_t data;
  data = lwlp.getData();
  temperature = data.temperature;
  pressure = data.presure;

  if (operationDone)
  {

    // reset flag
    operationDone = false;
    if (transmitFlag)
    {
      // the previous operation was transmission, listen for response
      // print the result
      if (transmissionState == RADIOLIB_ERR_NONE)
      {
        // packet was successfully sent
        // Serial.println(F("transmission finished!"));
        blinkLed(2);
      }
      else
      {
        // Serial.print(F("failed, code "));
        // Serial.println(transmissionState);
      }

      // listen for response
      radio.startReceive();
      transmitFlag = false;
    }
    else
    {
      size_t len = radio.getPacketLength();
      uint8_t byteArray[len];
      int state = radio.readData(byteArray, len);
      if (state == RADIOLIB_ERR_NONE)
      {
        if (len < 8)
        {
          Serial.print(byteArray[0]);
          Serial.print(" - ");
          Serial.println(byteArray[1]);
          if ((byteArray[0] == id.networkId) && (byteArray[1] == id.deviceId))
          {

            int cmd = byteArray[2];
            uint32_t vParams = byteArray[6];
            switch (cmd)
            {
            case 0:
              // request status
              Serial.println("request data dieksekusi");
              sendSta = true;
              break;
            case 1:
              // change relay
              Serial.println("request changeRelay dieksekusi");
              id.networkId = vParams;
              EEPROM.put(0, id);
              break;
            case 2:
              // ubah nodeId
              id.deviceId = vParams;
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
              updateRateSend = 1000 * id.periode;
              EEPROM.put(0, id);
              break;
            default:
              //
              break;
            }
            blinkLed(1);
          }
          blinkLed(1);
        }
      }
      radio.startReceive();
      transmitFlag = false;
    }
  }

  if (id.mode == 1)
  {
    int currPeriod = millis();
    if ((currPeriod - lastUpdate) > updateRateSend)
    {
      lastUpdate = currPeriod + generateRandom(100, 1000);
      sendStatus();
    }
  }
  if (sendSta)
  {
    sendSta = false;
    delay(1000);
    sendStatus();
  }

  if (digitalRead(READSTATUS))
  {
    shell.executeIfInput();
  }
}
