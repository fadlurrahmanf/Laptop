#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1 // Gunakan reset pin, -1 jika tidak digunakan

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  // Memulai komunikasi serial (untuk debugging)
  Serial.begin(115200);

  // Inisialisasi display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Alamat I2C 0x3C
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.clearDisplay();
  
  // Menampilkan teks
  display.setTextSize(1);             // Ukuran teks 1
  display.setTextColor(SSD1306_WHITE); // Warna teks putih
  display.setCursor(0, 0);            // Set posisi awal
  display.println(F("Hello, OLED!")); // Teks yang ditampilkan

  display.display(); // Mengirim buffer ke OLED
}

void loop() {
  // Tidak ada operasi di loop
}
