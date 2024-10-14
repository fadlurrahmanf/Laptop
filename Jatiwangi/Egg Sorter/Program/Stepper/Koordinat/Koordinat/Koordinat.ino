#define PUL 12  // Pin untuk sinyal pulsa (PUL)
#define DIR 13  // Pin untuk arah (DIR)
#define ENA 14  // Pin untuk enable (ENA)

int stepCount = 850;   // Jumlah langkah per putaran
int loops = 5;         // Jumlah loop
int delayPulse = 1000;  // Delay untuk sinyal pulsa (dalam mikrodetik)
bool majumundur = true; // Mengaktifkan gerakan maju-mundur

void setup() {
  // Atur pin sebagai output
  pinMode(PUL, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(ENA, OUTPUT);

  // Aktifkan driver dengan ENA (LOW = aktif, HIGH = non-aktif)
  digitalWrite(ENA, HIGH);  
  delay(2000);
  digitalWrite(ENA, LOW);  

  // Set initial states to prevent floating
  digitalWrite(DIR, LOW);  // Set initial direction
  digitalWrite(PUL, LOW);  // Set initial pulse
  delay(10000);
}

void loop() {
  // Gerakan maju
  digitalWrite(DIR, LOW);  // Tentukan arah maju
  for (int i = 0; i < loops; i++) {
    digitalWrite(DIR, LOW);  // Tentukan arah maju
    gerakanMotor(stepCount);
    delay(100);  // Tunggu sejenak antara setiap loop
  }
  delay(5000);  // Tunggu 5 detik sebelum mundur

  // Gerakan mundur jika majumundur diaktifkan
  if (majumundur) {
    digitalWrite(DIR, HIGH);  // Tentukan arah mundur
    for (int i = 0; i < loops; i++) {
      gerakanMotor(stepCount);
      delay(100);  // Tunggu sejenak antara setiap loop
    }
    delay(5000);  // Tunggu 5 detik sebelum maju lagi
  }
}

// Fungsi untuk menjalankan motor sebanyak langkah yang ditentukan
void gerakanMotor(int steps) {
  for (int x = 0; x < steps; x++) {
    digitalWrite(PUL, HIGH);
    delayMicroseconds(delayPulse); // Delay antara HIGH dan LOW
    digitalWrite(PUL, LOW);
    delayMicroseconds(delayPulse); // Delay antara LOW dan HIGH
  }
}
