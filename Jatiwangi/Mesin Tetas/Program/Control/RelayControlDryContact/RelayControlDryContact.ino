#define K1 16
#define K2 14

// #define I1 12
// #define I2 13

#include "fdrs_node_config.h"
#include <fdrs_node.h>

uint8_t currentRead = 0;
// uint8_t inputRead = 0;

void fdrs_recv_cb(DataReading theData) {
  DBG("ID: " + String(theData.id));
  DBG("Type: " + String(theData.t));
  DBG("Data: " + String(theData.d));

  if (theData.id == READING_ID) {
    DBG("ID Sesuai");
    if ((theData.d == 1 || theData.d == 0) && theData.id >= 256) {
      DBG("Data Valid");
      switch (theData.t) {
        case 1:
          DBG("Case 1 Aktif");
          if (theData.d == 1) {
            DBG("K1 Nyala");
            digitalWrite(K1, HIGH);
            currentRead = bitSet(currentRead, theData.t - 1);
          } else if (!theData.d) {
            DBG("K1 Mati");
            digitalWrite(K1, LOW);
            currentRead = bitClear(currentRead, theData.t - 1);
          }
          break;
        case 2:
          DBG("Case 2 Aktif");
          if (theData.d == 1) {
            DBG("K2 Nyala");
            digitalWrite(K2, HIGH);
            currentRead = bitSet(currentRead, theData.t - 1);
          } else if (!theData.d) {
            DBG("K2 Mati");
            digitalWrite(K2, LOW);
            currentRead = bitClear(currentRead, theData.t - 1);
          }
          break;
        default:
          break;
      }
    }
  }
  loadFDRS(currentRead, STATUS_T);
  if (sendFDRS()) {
    DBG("Big Success!");
  } else {
    DBG("Nope, not so much.");
  }
}
// void checkInput() {
//   uint8_t currentInput = 0;
//   if (digitalRead(I1)) {
//     currentInput = bitSet(currentInput, 0);  //01
//   } else if (!digitalRead(I1)) {
//     currentInput = bitClear(currentInput, 0);  //00
//   }

//   if (digitalRead(I2)) {
//     currentInput = bitSet(currentInput, 1);  //10
//   } else if (!digitalRead(I1)) {
//     currentInput = bitClear(currentInput, 1);  //00 11
//   }

//   // Serial.println(currentInput);
//   // if (currentInput == 0) {
//   //   digitalWrite(K1, LOW);
//   //   currentRead = bitClear(currentRead, 0);
//   // } else if (currentInput == 3) {
//   //   digitalWrite(K1, HIGH);
//   //   currentRead = bitSet(currentRead, 0);
//   // }

//   if (currentInput != inputRead) {
//     loadFDRS(currentInput, VOLTAGE_T);
//     if (sendFDRS()) {
//       DBG("Big Success!");
//       inputRead = currentInput;
//     } else {
//       DBG("Nope, not so much.");
//     }
//   }
// }

void setup() {
  beginFDRS();
  pingFDRS(1000);
  addFDRS(fdrs_recv_cb);
  subscribeFDRS(READING_ID);

  pinMode(K1, OUTPUT);
  pinMode(K2, OUTPUT);
  // pinMode(I1, INPUT);
  // pinMode(I2, INPUT);

  digitalWrite(K1, LOW);
  digitalWrite(K2, LOW);
  Serial.println("Starting");
  loadFDRS(currentRead, STATUS_T);
  sendFDRS();
    
}
void loop() {
  loopFDRS();
  // checkInput();
}
