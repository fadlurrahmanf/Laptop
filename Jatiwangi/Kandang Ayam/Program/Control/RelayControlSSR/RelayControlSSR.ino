#define K1 12
#define K2 13
#define K3 14
#define K4 16

#include "fdrs_node_config.h"
#include <fdrs_node.h>

uint8_t currentRead = 0;

void fdrs_recv_cb(DataReading theData) {
  DBG("ID: " + String(theData.id));
  DBG("Type: " + String(theData.t));
  DBG("Data: " + String(theData.d));

  if (theData.id == READING_ID) {
    if ((theData.d == 1 || theData.d == 0) && theData.id >= 256) {
      switch (theData.t) {
        case 1:
          if (theData.d == 1) {
            digitalWrite(K1, HIGH);
            currentRead = bitSet(currentRead, theData.t - 1);
          } else if (!theData.d) {
            digitalWrite(K1, LOW);
            currentRead = bitClear(currentRead, theData.t - 1);
          }
          break;
        case 2:
          if (theData.d == 1) {
            digitalWrite(K2, HIGH);
            currentRead = bitSet(currentRead, theData.t - 1);
          } else if (!theData.d) {
            digitalWrite(K2, LOW);
            currentRead = bitClear(currentRead, theData.t - 1);
          }
          break;
        case 3:
          if (theData.d == 1) {
            digitalWrite(K3, HIGH);
            currentRead = bitSet(currentRead, theData.t - 1);
          } else if (!theData.d) {
            digitalWrite(K3, LOW);
            currentRead = bitClear(currentRead, theData.t - 1);
          }
          break;
        case 4:
          if (theData.d == 1) {
            digitalWrite(K4, HIGH);
            currentRead = bitSet(currentRead, theData.t - 1);
          } else if (!theData.d) {
            digitalWrite(K4, LOW);
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

void setup() {
  beginFDRS();
  pingFDRS(1000);
  addFDRS(fdrs_recv_cb);
  subscribeFDRS(READING_ID);

  pinMode(K1, OUTPUT);
  pinMode(K2, OUTPUT);
  pinMode(K3, OUTPUT);
  pinMode(K4, OUTPUT);
  digitalWrite(K1, LOW);
  digitalWrite(K2, LOW);
  digitalWrite(K3, LOW);
  digitalWrite(K4, LOW);
}
void loop() {
  loopFDRS();
}
