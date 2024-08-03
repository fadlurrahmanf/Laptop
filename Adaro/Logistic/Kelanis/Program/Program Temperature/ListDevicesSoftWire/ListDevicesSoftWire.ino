#include <SoftWire.h>
#include <AsyncDelay.h>
#define LED_BUILTIN PC13
SoftWire Wire(PB6, PB7);


void setup(void)
{
#if F_CPU >= 12000000UL
  Serial.begin(115200);
#else
  Serial.begin(9600);
#endif

  Wire.setTimeout_ms(40);
  Wire.begin();

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // Set how long we are willing to wait for a device to respond
  Wire.setTimeout_ms(200);

  const uint8_t firstAddr = 1;
  const uint8_t lastAddr = 0x7F;
  Serial.println();
  Serial.print("Interrogating all addresses in range 0x");
  Serial.print(firstAddr, HEX);
  Serial.print(" - 0x");
  Serial.print(lastAddr, HEX);
  Serial.println(" (inclusive) ...");

  for (uint8_t addr = firstAddr; addr <= lastAddr; addr++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delayMicroseconds(50);

    uint8_t startResult = Wire.llStart((addr << 1) + 1); // Signal a read
    Serial.println(startResult);
    Wire.stop();

    if (startResult == 0) {
      Serial.print("\rDevice found at 0x");
      Serial.println(addr, HEX);
      Serial.flush();
    }
    digitalWrite(LED_BUILTIN, LOW);

    delay(50);
  }
  Serial.println("Finished");

}


void loop(void)
{
  ;
}
