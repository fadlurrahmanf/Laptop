                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                ) | (1 << CS10);
    tshift = 10;
  } else {
    error("Sample Rate Too Slow");
  }
  // divide by prescaler
  ticks >>= tshift;
  // set TOP for timer reset
  ICR1 = ticks - 1;
  // compare for ADC start
  OCR1B = 0;

  // multiply by prescaler
  ticks <<= tshift;

  // Sample interval in CPU clock ticks.
  meta->sampleInterval = ticks;
  meta->cpuFrequency = F_CPU;
  float sampleRate = (float)meta->cpuFrequency/meta->sampleInterval;
  Serial.print(F("Sample pins:"));
  for (uint8_t i = 0; i < meta->pinCount; i++) {
    Serial.print(' ');
    Serial.print(meta->pinNumber[i], DEC);
  }
  Serial.println();
  Serial.print(F("ADC bits: "));
  Serial.println(meta->recordEightBits ? 8 : 10);
  Serial.print(F("ADC clock kHz: "));
  Serial.println(meta->adcFrequency/1000);
  Serial.print(F("Sample Rate: "));
  Serial.println(sampleRate);
  Serial.print(F("Sample interval usec: "));
  Serial.println(1000000.0/sampleRate, 4);
}
//------------------------------------------------------------------------------
// enable ADC and timer1 interrupts
void adcStart() {
  // initialize ISR
  isrBufNeeded = true;
  isrOver = 0;
  adcindex = 1;

  // Clear any pending interrupt.
  ADCSRA |= 1 << ADIF;

  // Setup for first pin.
  ADMUX = adcmux[0];
  ADCSRB = adcsrb[0];
  ADCSRA = adcsra[0];

  // Enable timer1 interrupts.
  timerError = false;
  timerFlag = false;
  TCNT1 = 0;
  TIFR1 = 1 << OCF1B;
  TIMSK1 = 1 << OCIE1B;
}
//------------------------------------------------------------------------------
void adcStop() {
  TIMSK1 = 0;
  ADCSRA = 0;
}
//------------------------------------------------------------------------------
// Convert binary file to csv file.
void binaryToCsv() {
  uint8_t lastPct = 0;
  block_t buf;
  metadata_t* pm;
  uint32_t t0 = millis();
  char csvName[13];
  StdioStream csvStream;

  if (!binFile.isOpen()) {
    Serial.println(F("No current binary file"));
    return;
  }
  binFile.rewind();
  if (binFile.read(&buf , 512) != 512) {
    error("Read metadata failed");
  }
  // Create a new csv file.
  strcpy(csvName, binName);
  strcpy(&csvName[BASE_NAME_SIZE + 3], "csv");

  if (!csvStream.fopen(csvName, "w")) {
    error("open csvStream failed");
  }
  Serial.println();
  Serial.print(F("Writing: "));
  Serial.print(csvName);
  Serial.println(F(" - type any character to stop"));
  pm = (metadata_t*)&buf;
  csvStream.print(F("Interval,"));
  float intervalMicros = 1.0e6*pm->sampleInterval/(float)pm->cpuFrequency;
  csvStream.print(intervalMicros, 4);
  csvStream.println(F(",usec"));
  for (uint8_t i = 0; i < pm->pinCount; i++) {
    if (i) {
      csvStream.putc(',');
    }
    csvStream.print(F("pin"));
    csvStream.print(pm->pinNumber[i]);
  }
  csvStream.println();
  uint32_t tPct = millis();
  while (!Serial.available() && binFile.read(&buf, 512) == 512) {
    if (buf.count == 0) {
      break;
    }
    if (buf.overrun) {
      csvStream.print(F("OVERRUN,"));
      csvStream.println(buf.overrun);
    }
    for (uint16_t j = 0; j < buf.count; j += PIN_COUNT) {
      for (uint16_t i = 0; i < PIN_COUNT; i++) {
        if (i) {
          csvStream.putc(',');
        }
        csvStream.print(buf.data[i + j]);
      }
      csvStream.println();
    }
    if ((millis() - tPct) > 1000) {
      uint8_t pct = binFile.curPosition()/(binFile.fileSize()/100);
      if (pct != lastPct) {
        tPct = millis();
        lastPct = pct;
        Serial.print(pct, DEC);
        Serial.println('%');
      }
    }
    if (Serial.available()) {
      break;
    }
  }
  csvStream.fclose();
  Serial.print(F("Done: "));
  Serial.print(0.001*(millis() - t0));
  Serial.println(F(" Seconds"));
}
//------------------------------------------------------------------------------
// read data file and check for overruns
void checkOverrun() {
  bool headerPrinted = false;
  block_t buf;
  uint32_t bgnBlock, endBlock;
  uint32_t bn = 0;

  if (!binFile.isOpen()) {
    Serial.println(F("No current binary file"));
    return;
  }
  if (!binFile.contiguousRange(&bgnBlock, &endBlock)) {
    error("contiguousRange failed");
  }
  binFile.rewind();
  Serial.println();
  Serial.println(F("Checking overrun errors - type any character to stop"));
  if (binFile.read(&buf , 512) != 512) {
    error("Read metadata failed");
  }
  bn++;
  while (binFile.read(&buf, 512) == 512) {
    if (buf.count == 0) {
      break;
    }
    if (buf.overrun) {
      if (!headerPrinted) {
        Serial.println();
        Serial.println(F("Overruns:"));
        Serial.println(F("fileBlockNumber,sdBlockNumber,overrunCount"));
        headerPrinted = true;
      }
      Serial.print(bn);
      Serial.print(',');
      Serial.print(bgnBlock + bn);
      Serial.print(',');
      Serial.println(buf.overrun);
    }
    bn++;
  }
  if (!headerPrinted) {
    Serial.println(F("No errors found"));
  } else {
    Serial.println(F("Done"));
  }
}
//------------------------------------------------------------------------------
// dump data file to Serial
void dumpData() {
  block_t buf;
  if (!binFile.isOpen()) {
    Serial.println(F("No current binary file"));
    return;
  }
  binFile.rewind();
  if (binFile.read(&buf , 512) != 512) {
    error("Read metadata failed");
  }
  Serial.println();
  Serial.println(F("Type any character to stop"));
  delay(1000);
  while (!Serial.available() && binFile.read(&buf , 512) == 512) {
    if (buf.count == 0) {
      break;
    }
    if (buf.overrun) {
      Serial.print(F("OVERRUN,"));
      Serial.println(buf.overrun);
    }
    for (uint16_t i = 0; i < buf.count; i++) {
      Serial.print(buf.data[i], DEC);
      if ((i+1)%PIN_COUNT) {
        Serial.print(',');
      } else {
        Serial.println();
      }
    }
  }
  Serial.println(F("Done"));
}
//------------------------------------------------------------------------------
// log data
// max number of blocks to erase per erase call
uint32_t const ERASE_SIZE = 262144L;
void logData() {
  uint32_t bgnBlock, endBlock;

  // Allocate extra buffer space.
  block_t block[BUFFER_BLOCK_COUNT];

  Serial.println();

  // Initialize ADC and timer1.
  adcInit((metadata_t*) &block[0]);

  // Find unused file name.
  if (BASE_NAME_SIZE > 6) {
    error("FILE_BASE_NAME too long");
  }
  while (sd.exists(binName)) {
    if (binName[BASE_NAME_SIZE + 1] != '9') {
      binName[BASE_NAME_SIZE + 1]++;
    } else {
      binName[BASE_NAME_SIZE + 1] = '0';
      if (binName[BASE_NAME_SIZE] == '9') {
        error("Can't create file name");
      }
      binName[BASE_NAME_SIZE]++;
    }
  }
  // Delete old tmp file.
  if (sd.exists(TMP_FILE_NAME)) {
    Serial.println(F("Deleting tmp file"));
    if (!sd.remove(TMP_FILE_NAME)) {
      error("Can't remove tmp file");
    }
  }
  // Create new file.
  Serial.println(F("Creating new file"));
  binFile.close();
  if (!binFile.createContiguous(TMP_FILE_NAME, 512 * FILE_BLOCK_COUNT)) {
    error("createContiguous failed");
  }
  // Get the address of the file on the SD.
  if (!binFile.contiguousRange(&bgnBlock, &endBlock)) {
    error("contiguousRange failed");
  }
  // Use SdFat's internal buffer.
  uint8_t* cache = (uint8_t*)sd.vol()->cacheClear();
  if (cache == 0) {
    error("cacheClear failed");
  }

  // Flash erase all data in the file.
  Serial.println(F("Erasing all data"));
  uint32_t bgnErase = bgnBlock;
  uint32_t endErase;
  while (bgnErase < endBlock) {
    endErase = bgnErase + ERASE_SIZE;
    if (endErase > endBlock) {
      endErase = endBlock;
    }
    if (!sd.card()->erase(bgnErase, endErase)) {
      error("erase failed");
    }
    bgnErase = endErase + 1;
  }
  // Start a multiple block write.
  if (!sd.card()->writeStart(bgnBlock, FILE_BLOCK_COUNT)) {
    error("writeBegin failed");
  }
  // Write metadata.
  if (!sd.card()->writeData((uint8_t*)&block[0])) {
    error("Write metadata failed");
  }
  // Initialize queues.
  emptyHead = emptyTail = 0;
  fullHead = fullTail = 0;

  // Use SdFat buffer for one block.
  emptyQueue[emptyHead] = (block_t*)cache;
  emptyHead = queueNext(emptyHead);

  // Put rest of buffers in the empty queue.
  for (uint8_t i = 0; i < BUFFER_BLOCK_COUNT; i++) {
    emptyQueue[emptyHead] = &block[i];
    emptyHead = queueNext(emptyHead);
  }
  // Give SD time to prepare for big write.
  delay(1000);
  Serial.println(F("Logging - type any character to stop"));
  // Wait for Serial Idle.
  Serial.flush();
  delay(10);
  uint32_t bn = 1;
  uint32_t t0 = millis();
  uint32_t t1 = t0;
  uint32_t overruns = 0;
  uint32_t count = 0;
  uint32_t maxLatency = 0;

  // Start logging interrupts.
  adcStart();
  while (1) {
    if (fullHead != fullTail) {
      // Get address of block to write.
      block_t* pBlock = fullQueue[fullTail];

      // Write block to SD.
      uint32_t usec = micros();
      if (!sd.card()->writeData((uint8_t*)pBlock)) {
        error("write data failed");
      }
      usec = micros() - usec;
      t1 = millis();
      if (usec > maxLatency) {
        maxLatency = usec;
      }
      count += pBlock->count;

      // Add overruns and possibly light LED.
      if (pBlock->overrun) {
        overruns += pBlock->overrun;
        if (ERROR_LED_PIN >= 0) {
          digitalWrite(ERROR_LED_PIN, HIGH);
        }
      }
      // Move block to empty queue.
      emptyQueue[emptyHead] = pBlock;
      emptyHead = queueNext(emptyHead);
      fullTail = queueNext(fullTail);
      bn++;
      if (bn == FILE_BLOCK_COUNT) {
        // File full so stop ISR calls.
        adcStop();
        break;
      }
    }
    if (timerError) {
      error("Missed timer event - rate too high");
    }
    if (Serial.available()) {
      // Stop ISR calls.
      adcStop();
      if (isrBuf != 0 && isrBuf->count >= PIN_COUNT) {
        // Truncate to last complete sample.
        isrBuf->count = PIN_COUNT*(isrBuf->count/PIN_COUNT);
        // Put buffer in full queue.
        fullQueue[fullHead] = isrBuf;
        fullHead = queueNext(fullHead);
        isrBuf = 0;
      }
      if (fullHead == fullTail) {
        break;
      }
    }
  }
  if (!sd.card()->writeStop()) {
    error("writeStop failed");
  }
  // Truncate file if recording stopped early.
  if (bn != FILE_BLOCK_COUNT) {
    Serial.println(F("Truncating file"));
    if (!binFile.truncate(512L * bn)) {
      error("Can't truncate file");
    }
  }
  if (!binFile.rename(binName)) {
    error("Can't rename file");
  }
  Serial.print(F("File renamed: "));
  Serial.println(binName);
  Serial.print(F("Max block write usec: "));
  Serial.println(maxLatency);
  Serial.print(F("Record time sec: "));
  Serial.println(0.001*(t1 - t0), 3);
  Serial.print(F("Sample count: "));
  Serial.println(count/PIN_COUNT);
  Serial.print(F("Samples/sec: "));
  Serial.println((1000.0/PIN_COUNT)*count/(t1-t0));
  Serial.print(F("Overruns: "));
  Serial.println(overruns);
  Serial.println(F("Done"));
}
//------------------------------------------------------------------------------
void setup(void) {
  if (ERROR_LED_PIN >= 0) {
    pinMode(ERROR_LED_PIN, OUTPUT);
  }
  Serial.begin(9600);

  // Read the first sample pin to init the ADC.
  analogRead(PIN_LIST[0]);

  Serial.print(F("FreeStack: "));
  Serial.println(FreeStack());

  // Initialize at the highest speed supported by the board that is
  // not over 50 MHz. Try a lower speed if SPI errors occur.
  if (!sd.begin(SD_CS_PIN, SD_SCK_MHZ(50))) {
    sd.initErrorPrint();
    fatalBlink();
  }
}
//------------------------------------------------------------------------------
void loop(void) {
  // Read any Serial data.
  do {
    delay(10);
  } while (Serial.available() && Serial.read() >= 0);
  Serial.println();
  Serial.println(F("type:"));
  Serial.println(F("c - convert file to csv"));
  Serial.println(F("d - dump data to Serial"));
  Serial.println(F("e - overrun error details"));
  Serial.println(F("r - record ADC data"));

  while(!Serial.available()) {
    yield();
  }
  char c = tolower(Serial.read());
  if (ERROR_LED_PIN >= 0) {
    digitalWrite(ERROR_LED_PIN, LOW);
  }
  // Read any Serial data.
  do {
    delay(10);
  } while (Serial.available() && Serial.read() >= 0);

  if (c == 'c') {
    binaryToCsv();
  } else if (c == 'd') {
    dumpData();
  } else if (c == 'e') {
    checkOverrun();
  } else if (c == 'r') {
    logData();
  } else {
    Serial.println(F("Invalid entry"));
  }
}
#else  // __AVR__
#error This program is only for AVR.
#endif  // __AVR__