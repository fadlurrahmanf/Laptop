/*************************************************** 
 This is a library written for the Maxim MAX30105 Optical Smoke Detector
 It should also work with the MAX30102. However, the MAX30102 does not have a Green LED.

 These sensors use I2C to communicate, as well as a single (optional)
 interrupt line that is not currently supported in this driver.
 
 Written by Peter Jansen and Nathan Seidle (SparkFun)
 BSD license, all text above must be included in any redistribution.
 *****************************************************/

#pragma once

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <Wire.h>

#define MAX30105_ADDRESS          0x57 //7-bit I2C Address
//Note that MAX30102 has the same I2C address and Part ID

#define I2C_SPEED_STANDARD        100000
#define I2C_SPEED_FAST            400000

//Define the size of the I2C buffer based on the platform the user has
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)

  //I2C_BUFFER_LENGTH is defined in Wire.H
  #define I2C_BUFFER_LENGTH BUFFER_LENGTH

#elif defined(__SAMD21G18A__)

  //SAMD21 uses RingBuffer.h
  #define I2C_BUFFER_LENGTH SERIAL_BUFFER_SIZE

#else

  //The catch-all default is 32
  #define I2C_BUFFER_LENGTH 32

#endif

class MAX30105 {
 public: 
  MAX30105(void);

  boolean begin(TwoWire &wirePort = Wire, uint32_t i2cSpeed = I2C_SPEED_STANDARD, uint8_t i2caddr = MAX30105_ADDRESS);

  uint32_t getRed(void); //Returns immediate red value
  uint32_t getIR(void); //Returns immediate IR value
  uint32_t getGreen(void); //Returns immediate green value
  bool safeCheck(uint8_t maxTimeToCheck); //Given a max amount of time, check for new data

  // Configuration
  void softReset();
  void shutDown(); 
  void wakeUp(); 

  void setLEDMode(uint8_t mode);

  void setADCRange(uint8_t adcRange);
  void setSampleRate(uint8_t sampleRate);
  void setPulseWidth(uint8_t pulseWidth);

  void setPulseAmplitudeRed(uint8_t value);
  void setPulseAmplitudeIR(uint8_t value);
  void setPulseAmplitudeGreen(uint8_t value);
  void setPulseAmplitudeProximity(uint8_t value);

  void setProximityThreshold(uint8_t threshMSB);

  //Multi-led configuration mode (page 22)
  void enableSlot(uint8_t slotNumber, uint8_t device); //Given slot number, assign a device to slot
  void disableSlots(void);
  
  // Data Collection

  //Interrupts (page 13, 14)
  uint8_t getINT1(void); //Returns the main interrupt group
  uint8_t getINT2(void); //Returns the temp ready interrupt
  void enableAFULL(void); //Enable/disable individual interrupts
  void disableAFULL(void);
  void enableDATARDY(void);
  void disableDATARDY(void);
  void enableALCOVF(void);
  void disableALCOVF(void);
  void enablePROXINT(void);
  void disablePROXINT(void);
  void enableDIETEMPRDY(void);
  void disableDIETEMPRDY(void);

  //FIFO Configuration (page 18)
  void setFIFOAverage(uint8_t samples);
  void enableFIFORollover();
  void disableFIFORollover();
  void setFIFOAlmostFull(uint8_t samples);
  
  //FIFO Reading
  uint16_t check(void); //Checks for new data and fills FIFO
  uint8_t available(void); //Tells caller how many new samples are available (head - tail)
  void nextSample(void); //Advances the tail of the sense array
  uint32_t getFIFORed(void); //Returns the FIFO sample pointed to by tail
  uint32_t getFIFOIR(void); //Returns the FIFO sample pointed to by tail
  uint32_t getFIFOGreen(void); //Returns the FIFO sample pointed to by tail

  uint8_t getWritePointer(void);
  uint8_t getReadPointer(void);
  void clearFIFO(void); //Sets the read/write pointers to zero

  //Proximity Mode Interrupt Threshold
  void setPROXINTTHRESH(uint8_t val);

  // Die Temperature
  float readTemperature();
  float readTemperatureF();

  // Detecting ID/Revision
  uint8_t getRevisionID();
  uint8_t readPartID();  

  // Setup the IC with user selectable settings
  void setup(byte powerLevel = 0x1F, byte sampleAverage = 4, byte ledMode = 3, int sampleRate = 400, int pulseWidth = 411, int adcRange = 4096);

  // Low-level I2C communication
  uint8_t readRegister8(uint8_t address, uint8_t regMAM�9  ����������������������������������������������������� �������������������������������
���
���������������������
������������             �y    ����    ����    ���� 
   ����  ����� �����w�� ���
w  ���� � ������� ���    z  �                h�)�y8b��-�$1Ej�%QR#ӂ;��_�G���/N�D�O��;'��w���6�����&*e�[ޅ�n�b���~�"&���l�aG=ԽM>���[�f����s���}�~�'�㚵4`��'�i��G�' �:�֬��e���~�J�i=0N�:{w�O]�ivU��}��b/���!��`�/���W�����"?UE���ݺ �nZ(���T��'�ẋj&Ө�	,��:�9��7�F�
�`�
s��%	VA�,f��m,�q30r M'��<�7�)y�~ ��~ �NHP��b�����_��	LA僭��86�7Ah���+4gk�QzP@���T&��O�zz=`�zAM�
l0y'?�>e7�@��]��j��T�#���π���T�Á�~e��K`��^�ڛ;
nP�j��*���Y7Tt`� `+o����2{�Twȹl�*r����9�I� �8�\��k�B��'HA�RY��gЭ��Qm5"K�h�`��x�^)N�+��0#T�e��@