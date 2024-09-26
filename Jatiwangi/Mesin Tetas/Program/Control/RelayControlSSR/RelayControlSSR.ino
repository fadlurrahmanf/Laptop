#define K1 16

#include "fdrs_node_config.h"
#include <fdrs_node.h>

uint8_t currentRead = 0;

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
            digitalWrite(K1, LOW);
            currentRead = bitSet(currentRead, theData.t - 1);
          } else if (!theData.d) {
            DBG("K1 Mati");
            digitalWrite(K1, HIGH);
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
  digitalWrite(K1, HIGH);
}
void loop() {
  loopFDRS();
}
