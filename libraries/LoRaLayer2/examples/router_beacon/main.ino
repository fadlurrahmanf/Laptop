#include <Layer1_LoRa.h>
#include <LoRaLayer2.h>

#define LORA_CS         16
#define LORA_RST        15
#define LORA_IRQ        26
#define LORA_FREQ       921.5E6

#define LED             17

Layer1Class *Layer1;
LL2Class *LL2;

void setup()
{
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

  Layer1 = new Layer1Class();
  Layer1->setPins(LORA_CS, LORA_RST, LORA_IRQ);
  Layer1->setLoRaFrequency(LORA_FREQ);
  if (Layer1->init())
  {
    Serial.println(" --> LoRa initialized");
    LL2 = new LL2Class(Layer1); // initialize Layer2
    LL2->setLocalAddress(nodeIDstr.c_str()); // this should either be randomized or set using the wifi mac address
    LL2->setInterval(60000); // set to zero to disable routing packets
  }
  else
  {
    Serial.println(" --> Failed to initialize LoRa");
  }
}

void loop()
{    
  LL2->daemon();
}
