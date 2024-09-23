#include <stdio.h>
#include <stdlib.h>

//LoRaLayer2
#include <Layer1_LoRa.h>
#include <LoRaLayer2.h>

#define LL2_DEBUG
#define DATAGRAM_HEADER 5

#define LORA_CS         16
#define LORA_RST        15
#define LORA_IRQ        26
#define LORA_FREQ       921.5E6

#define LED             17

#define TX_POWER 20


char MAC[9] = "c0d3f00d";
uint8_t LOCAL_ADDRESS[ADDR_LENGTH] = {0xc0, 0xd3, 0xf0, 0x0d};
uint8_t RECEIVER[ADDR_LENGTH] = {0xd4, 0xe6, 0xe2, 0xe0};

Layer1Class *Layer1;
LL2Class *LL2;

int counter = 0;
long lastTransmit;

void setup() {

  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  delay(200);
  uint64_t nodeId = ESP.getEfuseMac();
  unsigned long long1 = (unsigned long)((nodeId & 0xFFFF0000) >> 16 );
  unsigned long long2 = (unsigned long)((nodeId & 0x0000FFFF));
  String nodeIDstr = String(long1, HEX) + String(long2, HEX); //
  // Serial.printf("\nCHIP MAC: %012llx\n", ESP.getEfuseMac());
  Serial.println("");
  Serial.println(nodeIDstr);
  Serial.println("");

  Serial.println("* Initializing LoRa...");
  Serial.println("LoRa Sender");

  Layer1 = new Layer1Class();
  Layer1->setPins(LORA_CS, LORA_RST, LORA_IRQ);
  Layer1->setTxPower(TX_POWER);
  Layer1->setLoRaFrequency(LORA_FREQ);
  if (Layer1->init())
  {
    Serial.println(" --> Layer1 initialized");
    LL2 = new LL2Class(Layer1); // initialize Layer2
    LL2->setLocalAddress(nodeIDstr.c_str()); // this should either be randomized or set using the wifi mac address
    LL2->setInterval(60000); // set to zero to disable routing packets
    if (LL2->init() == 0){
      Serial.println(" --> LoRaLayer2 initialized");
    }
    else{
      Serial.println(" --> Failed to initialize LoRaLayer2");
    }
  }
  else
  {
    Serial.println(" --> Failed to initialize LoRa");
  }
  lastTransmit = Layer1Class::getTime();
}

void loop() {
  LL2->daemon();
  int msglen = 0;
  int datagramsize = 0;

  struct Datagram datagram; 
  if (Layer1Class::getTime() - lastTransmit >= 10000){
    msglen = sprintf((char*)datagram.message, "%s,%i", "hello", counter);
    memcpy(datagram.destination, RECEIVER, ADDR_LENGTH);
    datagram.type = 's'; // can be anything, but 's' for 'sensor'

    datagramsize = msglen + DATAGRAM_HEADER;

    // Send packet
    LL2->writeData(datagram, datagramsize);
    counter++;
    Serial.println((char*)datagram.message);

    lastTransmit = Layer1Class::getTime();
  }
}
