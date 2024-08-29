#include <RadioLib.h>
#include <IWatchdog.h>
#include <EEPROM.h>
#include <CayenneLPP.h>
#include <SimpleSerialShell.h>
// #include "PCF8574.h"

// Board PCB_STM32G030C8T6
#define LED PC13
#define NSS PA4
#define DIO0 PB1
#define DIO1 PA15
#define NRST PA3
#define LORA_ON PA11

// // IO extender PCF8574 I2C address
// #define ADDI2C1 0x20
// #define ADDI2C2 0x21

#define CTRBYTE 0x01

// #define STATUSPIN PB10
// #define READSTATUS PB11

// Definisi array untuk pin output
const uint16_t outputPins[] = {
    PB6,  // INPUT1
    PB7,  // INPUT2
    // PA8,  // OUTPUT3
    // PB15, // OUTPUT4
    // PB13, // OUTPUT5
    // PB12, // OUTPUT6
    // PB11, // OUTPUT7
    // PB10  // OUTPUT8
};

// Definisi array untuk pin input
const uint16_t inputPins[] = {
    // PB6,  // INPUT1
    // PB7,  // INPUT2
    PB8,  // INPUT3
    PB9  // INPUT4
    // PC14, // INPUT5
    // PC15, // INPUT6
    // PB4,  // INPUT7
    // PB5   // INPUT8
};

SX1276 radio = new Module(NSS, DIO0, NRST, DIO1);
// PCF8574 PCF1(ADDI2C1);
// PCF8574 PCF2(ADDI2C2);

typedef struct
{
    uint8_t networkId;
    uint8_t deviceId; // store this nodeId
    uint8_t cmd;      // type parameter
    uint32_t vParams; // temperature maybe?
} Payload;
Payload theData;

typedef struct
{
    uint8_t configId;
    uint8_t typeId;
    uint8_t networkId;
    uint16_t deviceId;
    uint8_t mode;
    uint8_t periode;
} Params;
Params id;

uint8_t updatePeriode = 120;
uint32_t updateRateSend = updatePeriode * 1000;
long lastUpdate;
uint8_t statusRelay = 0;
bool sendSta = false;
uint8_t channelNum;
bool changeR = false;
uint8_t mode = 1;
uint8_t deviceId = 5;
uint8_t networkId = 4;
uint8_t typeId = 4;
uint8_t lastRead = 0;
uint8_t currentRead = 0;

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

// save transmission states between loops
int transmissionState = RADIOLIB_ERR_NONE;

// flag to indicate transmission or reception state
bool transmitFlag = false;

// flag to indicate that a packet was sent or received
volatile bool operationDone = false;

// this function is called when a complete packet
// is received by the module
// IMPORTANT: this function MUST be 'void' type
//            and MUST NOT have any arguments!
void setFlag(void)
{
    // we got a packet, set the flag
    operationDone = true;
}

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

// void changeRelay(uint8_t vParams) { // jika vParams 0x03
//   vParams = vParams & 0x0FFFFFF;
//   for (int i = 0; i < 8; i++) { //1100 0000 urutan loop
//     if (bitRead(vParams, i) != bitRead(statusRelay,i)) { // vParams = 1100 0000 dan statusRelay = 1000 0000
//       PCF1.write(i, !bitRead(vParams, i)); // hanya 0100 0000 saja yg d run
//       delay(1000);
//     }
//   }
//   statusRelay = vParams;
// }

void changeRelay(uint8_t vParams)
{ // jika vParams 0x03
    vParams = vParams & 0x0FFFFFF;
    for (int i = 0; i < sizeof(outputPins) / sizeof(outputPins[0]); i++)
    {
        digitalWrite(outputPins[i], bitRead(vParams, i));
        delay(100);
    }
    statusRelay = vParams;
}

void sendStatus(uint8_t in)
{
    theData.networkId = id.networkId;
    theData.deviceId = id.deviceId;
    theData.cmd = 1;
    theData.vParams = (statusRelay * 256 + lastRead);

    // if (digitalRead(READSTATUS))
    // {
    //     Serial.print(F("theData: "));
    //     Serial.print(theData.networkId);
    //     Serial.print(F(","));
    //     Serial.print(theData.deviceId);
    //     Serial.print(F(","));
    //     Serial.print(theData.cmd);
    //     Serial.print(F(","));
    //     Serial.println(theData.vParams);
    // }
    byte len = sizeof(theData);
    byte byteArray[len + 1];
    memcpy(byteArray, (const void *)(&theData), len);
    byteArray[len] = calculateCRC(byteArray, len);
    transmissionState = radio.startTransmit(byteArray, len + 1);
    transmitFlag = true;
    blinkLed(2);
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

////////////////////////////////////////////////////////////////////////////////
int checkInput()
{
    currentRead = 255;
    for (int i = 0; i < sizeof(inputPins) / sizeof(inputPins[0]); i++)
    {
        int state = digitalRead(inputPins[i]);
        if (!state)
        {
            currentRead = currentRead - (pow(2, i));
        }
    }
}

void dumpByteArray(const byte *byteArray, const byte arraySize)
{
    for (int i = 0; i < arraySize; i++)
    {
        Serial.print("0x");
        if (byteArray[i] < 0x10)
            Serial.print("0");
        Serial.print(byteArray[i], HEX);
        Serial.print(", ");
    }
    Serial.println();
}

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH);
    pinMode(LORA_ON, OUTPUT);
    digitalWrite(LORA_ON, LOW);
    delay(10);

    // PCF1.begin();
    // PCF2.begin();

    if (IWatchdog.isReset(true))
    {
        // LED blinks to indicate reset
        blinkLed(10);
    }

    EEPROM.get(0, id);
    if (id.configId == CTRBYTE)
    {
        networkId = id.networkId;
        deviceId = id.deviceId;
        updatePeriode = id.periode;
        mode = id.mode;
        updateRateSend = 1000 * id.periode;
    }
    else
    {
        id.configId = CTRBYTE;
        id.networkId = networkId;
        id.deviceId = deviceId;
        id.mode = mode;
        id.periode = updatePeriode;
        updateRateSend = updatePeriode * 1000;
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
    blinkLed(3);

    //   pinMode(STATUSPIN, OUTPUT);
    //   digitalWrite(STATUSPIN, HIGH);
    //   pinMode(READSTATUS, INPUT);

    // Atur pin output
    for (int i = 0; i < sizeof(outputPins) / sizeof(outputPins[0]); i++)
    {
        pinMode(outputPins[i], OUTPUT);
    }

    // Atur pin input
    for (int i = 0; i < sizeof(inputPins) / sizeof(inputPins[0]); i++)
    {
        pinMode(inputPins[i], INPUT);
    }

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

    checkInput();
    if (currentRead != lastRead)
    {
        lastRead = currentRead;
        sendStatus(0);
    }

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
            // if (digitalRead(READSTATUS))
            // {
            //     dumpByteArray(byteArray, len + 1);
            // }
            if (state == RADIOLIB_ERR_NONE)
            {
                if (len < 8)
                {
                    if ((byteArray[0] == id.networkId) && (byteArray[1] == id.deviceId))
                    {
                        int cmd = byteArray[2];
                        uint32_t vParams = byteArray[6];
                        // uint32_t vParams = byteArray[3]<<24 + byteArray[4]<<16 + byteArray[5]<<8 + byteArray[6];
                        switch (cmd)
                        {
                        case 0:
                            // request status
                            // if (digitalRead(READSTATUS))
                            // {
                            //     Serial.println("request data dieksekusi");
                            // }
                            sendSta = true;
                            break;
                        case 1:
                            // change relay
                            // if (digitalRead(READSTATUS))
                            // {
                            //     Serial.println("request changeRelay dieksekusi");
                            // }
                            changeR = true;
                            channelNum = vParams;
                            break;
                        case 2:
                            // ubah nodeId
                            id.networkId = vParams;
                            EEPROM.put(0, id);
                            break;
                        case 3:
                            // ubah nodeId
                            id.deviceId = vParams;
                            EEPROM.put(0, id);
                            break;
                        case 4:
                            // ubah mode
                            id.mode = vParams;
                            EEPROM.put(0, id);
                            break;
                        case 5:
                            // ubah periode
                            id.periode = vParams;
                            updateRateSend = 1000 * id.periode;
                            EEPROM.put(0, id);
                            break;
                        default:
                            //
                            break;
                        }
                    }
                    blinkLed(2);
                }
            }
            radio.startReceive();
            transmitFlag = false;
        }
    }

    if (id.mode == 1)
    {
        if ((millis() - lastUpdate) > updateRateSend)
        {
            lastUpdate = millis();
            sendStatus(1);
        }
    }

    if (sendSta)
    {
        sendSta = false;
        sendStatus(1);
    }

    if (changeR)
    {
        changeR = false;
        changeRelay(channelNum);
        delay(2000);
        checkInput();
        if (currentRead != lastRead)
        {
            lastRead = currentRead;
        }
        sendStatus(1);
    }

    // if (digitalRead(READSTATUS))
    // {
    //     shell.executeIfInput();
    // }
}
