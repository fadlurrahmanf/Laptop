#include "hw_timer.h"
#include "fdrs_node_config.h"
#include <fdrs_node.h>

const byte zcPin = 12;
const byte pwmPin = 13;
int val = 0;
int prevVal = 0;  // Variabel untuk menyimpan nilai sebelumnya
unsigned long lastReceiveTime = 0;
const unsigned long debounceDelay = 1000;  // 1 detik delay
byte fade = 1;
byte state = 1;
byte tarBrightness = 255;
byte curBrightness = 0;
byte zcState = 0;  // 0 = ready; 1 = processing;

void fdrs_recv_cb(DataReading theData) {
  DBG("ID: " + String(theData.id));
  DBG("Type: " + String(theData.t));
  DBG("Data: " + String(theData.d));
  if(theData.id == READING_ID){
    val = (int)theData.d;
  }
  
}

// Deklarasi prototipe fungsi
//void ICACHE_RAM_ATTR dimTimerISR(); //jika ini diaktifkan maka void ICACHE_RAM_ATTR dimTimerISR() pindah kebawah void loop
//void ICACHE_RAM_ATTR zcDetectISR(); //jika ini diaktifkan maka void void ICACHE_RAM_ATTR zcDetectISR() pindah kebawah void ICACHE_RAM_ATTR dimTimerISR()
void ICACHE_RAM_ATTR dimTimerISR() {
  if (fade == 1) {
    if (curBrightness > tarBrightness || (state == 0 && curBrightness > 0)) {
      --curBrightness;
    } else if (curBrightness < tarBrightness && state == 1 && curBrightness < 255) {
      ++curBrightness;
    }
  } else {
    if (state == 1) {
      curBrightness = tarBrightness;
    } else {
      curBrightness = 0;
    }
  }

  if (curBrightness == 0) {
    state = 0;
    digitalWrite(pwmPin, 0);
  } else if (curBrightness == 255) {
    state = 1;
    digitalWrite(pwmPin, 1);
  } else {
    digitalWrite(pwmPin, 1);
  }

  zcState = 0;
}

void ICACHE_RAM_ATTR zcDetectISR() {
  if (zcState == 0) {
    zcState = 1;

    if (curBrightness < 255 && curBrightness > 0) {
      digitalWrite(pwmPin, 0);

      int dimDelay = 30 * (255 - curBrightness) + 400;  // 400
      hw_timer_arm(dimDelay);
    }
  }
}


void setup() {
  Serial.begin(115200);
  beginFDRS();
  pingFDRS(1000);
  addFDRS(fdrs_recv_cb);
  subscribeFDRS(READING_ID);
  pinMode(zcPin, INPUT_PULLUP);
  pinMode(pwmPin, OUTPUT);
  attachInterrupt(zcPin, zcDetectISR, RISING);  // Attach an Interupt to Pin 2 (interupt 0) for Zero Cross Detection
  hw_timer_init(NMI_SOURCE, 0);
  hw_timer_set_func(dimTimerISR);
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentMillis = millis();

  // Hanya proses data jika berbeda dari data sebelumnya dan ada delay
  if (val != prevVal && (currentMillis - lastReceiveTime > debounceDelay)) {
    tarBrightness = val;
    Serial.println(tarBrightness);
    prevVal = val;                    // Simpan nilai baru sebagai nilai sebelumnya
    lastReceiveTime = currentMillis;  // Reset waktu penerimaan
  }

  loopFDRS();
}
