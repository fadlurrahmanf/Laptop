{"expect_ct":[],"sts":[{"expiry":1704718954.491781,"host":"C47LSS67LOPR/nzUq66AFtkSkYv/QndegwCOfDtIPDU=","mode":"force-https","sts_include_subdomains":false,"sts_observed":1673182954.491787},{"expiry":1702647756.231993,"host":"Hi4bEdMq563Qsqn4sVyUls/uVk7U80IxMa3wyWVUqWU=","mode":"force-https","sts_include_subdomains":false,"sts_observed":1671111756.232},{"expiry":1705884471.199463,"host":"OuKlWsMW1dkkbI1X/oi6o0Y95ZNSWnSoeaIXAEYPlv4=","mode":"force-https","sts_include_subdomains":true,"sts_observed":1674348471.199466},{"expiry":1705111798.221726,"host":"Q4G6aIdezq0rDi89KmjqFSp7cn3C9gwZwsLa7+P5K38=","mode":"force-https","sts_include_subdomains":false,"sts_observed":1673575798.221728},{"expiry":1705884526.268336,"host":"o2oGFfZkps1NfT6asVPXqcBtIJRMNNqoO1jKhddjlmY=","mode":"force-https","sts_include_subdomains":false,"sts_observed":1674348526.268344},{"expiry":1705881062.066187,"host":"6Turz/dyrfpEyob9ip5MOi6TNRgdxLvniY94TmP8nX8=","mode":"force-https","sts_include_subdomains":false,"sts_observed":1674345062.066189},{"expiry":1705725903.841684,"host":"+RF8qPV/GSCVzxCzVhnVv/EK1dVO9QxZSHrOr6TxzzM=","mode":"force-https","sts_include_subdomains":false,"sts_observed":1674189903.841688}],"version":2}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        {"expect_ct":[],"sts":[{"expiry":1704718954.491781,"host":"C47LSS67LOPR/nzUq66AFtkSkYv/QndegwCOfDtIPDU=","mode":"force-https","sts_include_subdomains":false,"sts_observed":1673182954.491787},{"expiry":1702647756.231993,"host":"Hi4bEdMq563Qsqn4sVyUls/uVk7U80IxMa3wyWVUqWU=","mode":"force-https","sts_include_subdomains":false,"sts_observed":1671111756.232},{"expiry":1705884471.199463,"host":"OuKlWsMW1dkkbI1X/oi6o0Y95ZNSWnSoeaIXAEYPlv4=","mode":"force-https","sts_include_subdomains":true,"sts_observed":1674348471.199466},{"expiry":1705111798.221726,"host":"Q4G6aIdezq0rDi89KmjqFSp7cn3C9gwZwsLa7+P5K38=","mode":"force-https","sts_include_subdomains":false,"sts_observed":1673575798.221728},{"expiry":1705884544.286528,"host":"o2oGFfZkps1NfT6asVPXqcBtIJRMNNqoO1jKhddjlmY=","mode":"force-https","sts_include_subdomains":false,"sts_observed":1674348544.286535},{"expiry":1705881062.066187,"host":"6Turz/dyrfpEyob9ip5MOi6TNRgdxLvniY94TmP8nX8=","mode":"force-https","sts_include_subdomains":false,"sts_observed":1674345062.066189},{"expiry":1705725903.841684,"host":"+RF8qPV/GSCVzxCzVhnVv/EK1dVO9QxZSHrOr6TxzzM=","mode":"force-https","sts_include_subdomains":false,"sts_observed":1674189903.841688}],"version":2}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        ebug) {
    debugStream->print(F("SIM800L : doPost() - HTTP status "));
    debugStream->println(httpRC);
  }

  if(httpRC == 200) {
    // Get the size of the data to receive
    dataSize = 0;
    for(uint16_t i = 0; (internalBuffer[idxBase + 19 + i] - '0') >= 0 && (internalBuffer[idxBase + 19 + i] - '0') <= 9; i++) {
      if(i != 0) {
        dataSize = dataSize * 10;
      }
      dataSize += (internalBuffer[idxBase + 19 + i] - '0');
    }
  
    if(enableDebug) {
      debugStream->print(F("SIM800L : doPost() - Data size received of "));
      debugStream->print(dataSize);
      debugStream->println(F(" bytes"));
    }
  
    // Ask for reading and detect the start of the reading...
    sendCommand_P(AT_CMD_HTTPREAD);
    if(!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_HTTPREAD, 2)) {
      return 705;
    }
  
    // Read number of bytes defined in the dataSize
    for(uint16_t i = 0; i < dataSize && i < recvBufferSize; i++) {
      while(!stream->available());
      if(stream->available()) {
        // Load the next char
        recvBuffer[i] = stream->read();
        // If the character is CR or LF, ignore it (it's probably part of the module communication schema)
        if((recvBuffer[i] == '\r') || (recvBuffer[i] == '\n')) {
          i--;
        }
      }
    }
  
    if(recvBufferSize < dataSize) {
      dataSize = recvBufferSize;
      if(enableDebug) {
        debugStream->println(F("SIM800L : doPost() - Buffer overflow while loading data from HTTP. Keep only first bytes..."));
      }
    }
  
    // We are expecting a final OK
    if(!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK)) {
      if(enableDebug) debugStream->println(F("SIM800L : doPost() - Invalid end of data while reading HTTP result from the module"));
      return 705;
    }
  
    if(enableDebug) {
      debugStream->print(F("SIM800L : doPost() - Received from HTTP POST : "));
      debugStream->println(recvBuffer);
    }
  }

  // Terminate HTTP/S session
  uint16_t termRC = terminateHTTP();
  if(termRC > 0) {
    return termRC;
  }

  return httpRC;
}

/**
 * Do HTTP/S GET on a specific URL
 */
uint16_t SIM800L::doGet(const char* url, uint16_t serverReadTimeoutMs) {
  // Cleanup the receive buffer
  initRecvBuffer();
  dataSize = 0;
  
  // Initiate HTTP/S session
  uint16_t initRC = initiateHTTP(url);
  if(initRC > 0) {
    return initRC;
  }

  // Start HTTP GET action
  sendCommand_P(AT_CMD_HTTPACTION0);
  if(!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK)) {
    if(enableDebug) debugStream->println(F("SIM800L : doGet() - Unable to initiate GET action"));
    return 703;
  }

  // Wait answer from the server
  if(!readResponse(serverReadTimeoutMs)) {
    if(enableDebug) debugStream->println(F("SIM800L : doGet() - Server timeout"));
    return 408;
  }

  // Extract status information
  int16_t idxBase = strIndex(internalBuffer, "+HTTPACTION: 0,");
  if(idxBase < 0) {
    if(enableDebug) debugStream->println(F("SIM800L : doGet() - Invalid answer on HTTP GET"));
    return 703;
  }

  // Get the HTTP return code
  uint16_t httpRC = 0;
  httpRC += (internalBuffer[idxBase + 15] - '0') * 100;
  httpRC += (internalBuffer[idxBase + 16] - '0') * 10;
  httpRC += (internalBuffer[idxBase + 17] - '0') * 1;

  if(enableDebug) {
    debugStream->print(F("SIM800L : doGet() - HTTP status "));
    debugStream->println(httpRC);
  }

  if(httpRC == 200) {
    // Get the size of the data to receive
    dataSize = 0;
    for(uint16_t i = 0; (internalBuffer[idxBase + 19 + i] - '0') >= 0 && (internalBuffer[idxBase + 19 + i] - '0') <= 9; i++) {
      if(i != 0) {
        dataSize = dataSize * 10;
      }
      dataSize += (internalBuffer[idxBase + 19 + i] - '0');
    }
  
    if(enableDebug) {
      debugStream->print(F("SIM800L : doGet() - Data size received of "));
      debugStream->print(dataSize);
      debugStream->println(F(" bytes"));
    }
  
    // Ask for reading and detect the start of the reading...
    sendCommand_P(AT_CMD_HTTPREAD);
    if(!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_HTTPREAD, 2)) {
      return 705;
    }
  
    // Read number of bytes defined in the dataSize
    for(uint16_t i = 0; i < dataSize && i < recvBufferSize; i++) {
      while(!stream->available());
      if(stream->available()) {
        // Load the next char
        recvBuffer[i] = stream->read();
        // If the character is CR or LF, ignore it (it's probably part of the module communication schema)
        if((recvBuffer[i] == '\r') || (recvBuffer[i] == '\n')) {
          i--;
        }
      }
    }
  
    if(recvBufferSize < dataSize) {
      dataSize = recvBufferSize;
      if(enableDebug) {
        debugStream->println(F("SIM800L : doGet() - Buffer overflow while loading data from HTTP. Keep only first bytes..."));
      }
    }
  
    // We are expecting a final OK
    if(!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK)) {
      if(enableDebug) debugStream->println(F("SIM800L : doGet() - Invalid end of data while reading HTTP result from the module"));
      return 705;
    }
  
    if(enableDebug) {
      debugStream->print(F("SIM800L : doGet() - Received from HTTP GET : "));
      debugStream->println(recvBuffer);
    }
  }

  // Terminate HTTP/S session
  uint16_t termRC = terminateHTTP();
  if(termRC > 0) {
    return termRC;
  }

  return httpRC;
}

/**
 * Meta method to initiate the HTTP/S session on the module
 */
uint16_t SIM800L::initiateHTTP(const char* url) {
  // Init HTTP connection
  sendCommand_P(AT_CMD_HTTPINIT);
  if(!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK)) {
    if(enableDebug) debugStream->println(F("SIM800L : initiateHTTP() - Unable to init HTTP"));
    return 701;
  }
  
  // Use the GPRS bearer
  sendCommand_P(AT_CMD_HTTPPARA_CID);
  if(!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK)) {
    if(enableDebug) debugStream->println(F("SIM800L : initiateHTTP() - Unable to define bearer"));
    return 702;
  }

  // Define URL to look for
  sendCommand_P(AT_CMD_HTTPPARA_URL, url);
  if(!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK)) {
    if(enableDebug) debugStream->println(F("SIM800L : initiateHTTP() - Unable to define the URL"));
    return 702;
  }

  // HTTP or HTTPS
  if(strIndex(url, "https://") == 0) {
    sendCommand_P(AT_CMD_HTTPSSL_Y);
    if(!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK)) {
      if(enableDebug) debugStream->println(F("SIM800L : initiateHTTP() - Unable to switch to HTTPS"));
      return 702;
    }
  } else {
    sendCommand_P(AT_CMD_HTTPSSL_N);
    if(!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK)) {
      if(enableDebug) debugStream->println(F("SIM800L : initiateHTTP() - Unable to switch to HTTP"));
      return 702;
    }
  }

  return 0;
}

/**
 * Meta method to terminate the HTTP/S session on the module
 */
uint16_t SIM800L::terminateHTTP() {
  // Close HTTP connection
  sendCommand_P(AT_CMD_HTTPTERM);
  if(!readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK)) {
    if(enableDebug) debugStream->println(F("SIM800L : terminateHTTP() - Unable to close HTTP session"));
    return 706;
  }
  return 0;
}

/**
 * Force a reset of the module
 */
void SIM800L::reset() {
  if(enableDebug) debugStream->println(F("SIM800L : Reset"));
  
  // Reset the device
  digitalWrite(pinReset, HIGH);
  delay(500);
  digitalWrite(pinReset, LOW);
  delay(500);
  digitalWrite(pinReset, HIGH);
  delay(1000);

  // Purge the serial
  stream->flush();
  while (stream->available()) {
    stream->read();
  }
  
}

/**
 * Return the size of data received after the last successful HTTP connection
 */
uint8_t SIM800L::getDataSizeReceived() {
  return dataSize;
}

/**
 * Return the buffer of data received after the last successful HTTP connection
 */
char* SIM800L::getDataReceived() {
  return recvBuffer;
}

/**
 * Status function: Check if AT command works
 */
bool SIM800L::isReady() {
  sendCommand_P(AT_CMD_BASE);
  return readResponseCheckAnswer_P(DEFAULT_TIMEOUT, AT_RSP_OK);
}

/**
 * Status function: Check the power mode
 */
PowerMode SIM800L::getPowerMode() {
  sendCommand_P(AT_CMD_CFUN_TEST);
  if(readResponse(DEFAULT_TIMEOUT)) {
    // Check if there is an error
    int16_t errIdx = strIndex(internalBuffer, "ERROR");
    if(errIdx > 0) {
      return POW_ERROR;
    }

    // Extract the value
    int16_t idx = strIndex(internalBuffer, "+CFUN: ");
    char value = internalBuffer[idx + 7];

    // Prepare the clear output
    switch(value) {
      case '0' : return MINIMUM;
      case '1' : return NORMAL;
      case '4' : return SLEEP;
      default  : return POW_UNKNOWN;
    }
  }
  return POW_ERROR;
}

/**
 * Status function: Get version of the module
 */
char* SIM800L::getVersion() {
  sendCommand_P(AT_CMD_ATI);
  if(readResponse(DEFAULT_TIMEOUT)) {
    // Extract the value
    int16_t idx = strIndex(internalBuffer, "SIM");
    int16_t idxEnd = strIndex(internalBuffer, "\r", idx+1);

    // Store it on the recv buffer (not used at the moment)
    initRecvBuffer();
    for(uint16_t i = 0; i < idxEnd - idx; i++) {
      recvBuffer[i] = internalBuffer[idx + i];
    }
    return getDataReceived();
  } else {
    return NULL;
  }
}

/**
 * Status function: Get firmware version
 */
char* SIM800L::getFirmware() {
  sendCommand_P(AT_CMD_GMR);
  if(readResponse(DEFAULT_TIMEOUT)) {
    // Extract the value
    int16_t idx = strIndex(internalBuffer, "AT+GMR") + 9;
    int16_t idxEnd = strIndex(internalBuffer, "\r", idx+1);
    
    // Store it on the recv buffer (not used at the moment)
    initRecvBuffer();
    for(uint16_t i = 0; i < idxEnd - idx; i++) {
      recvBuffer[i] = internalBuffer[idx + i];
    }
    return getDataReceived();
  } else {
    return NULL;
  }
}

/**
 * Status function: Check if the module is registered on the network
 */
NetworkRegistration SIM800L::getRegistrationStatus() {
  sendCommand_P(AT_CMD_CREG_TEST);
  if(readResponse(DEFAULT_TIMEOUT)) {
    // Check if there is an error
    int16_t errIdx = strIndex(internalBuffer, "ERROR");
    if(errIdx > 0) {
      return NET_ERROR;
    }

    // Extract the value
    int16_t idx = strIndex(internalBuffer, "+CREG: ");
    char value = internalBuffer[idx + 9];
  
    // Prepare the clear output
    switch(value) {
      case '0' : return NOT_REGISTERED;
      case '1' : return REGISTERED_HOME;
      case '2' : return SEARCHING;
      case '3' : return DENIED;
      case '5' : return REGISTERED_ROAMING;
      default  : return NET_UNKNOWN;
    }
  }
  
  return NET_ERROR;
}

/**
 * Setup the GPRS connectivity
 * As input, give the APN string of the operator
 */
bool SIM800L::setupGPRS(const char* apn) {
  // Prepare the GPRS connection as the bearer
  sendCommand_P(AT_CMD_SAPBR_GPRS);
  if(!readResponseCheckAnswer_P(20000, AT_RSP_OK)) {
    return false;
  }

  // Set the config of the bearer with the APN
  sendCommand_P(AT_CMD_SAPBR_APN, apn);
  return readResponseCheckAnswer_P(20000, AT_RSP_OK);
}

/**
 * Open the GPRS connectivity
 */
bool SIM800L::connectGPRS() {
  sendCommand_P(AT_CMD_SAPBR1);
  // Timout is max 85 seconds according to SIM800 specifications
  // We will wait for 65s to be within uint16_t
  return readResponseCheckAnswer_P(65000, AT_RSP_OK);
}

/**
 * Close the GPRS connectivity
 */
bool SIM800L::disconnectGPRS() {
  sendCommand_P(AT_CMD_SAPBR0);
  // Timout is max 65 seconds according to SIM800 specifications
  return readResponseCheckAnswer_P(65000, AT_RSP_OK);
}

/**
 * Define the power mode
 * Available : MINIMUM, NORMAL, SLEEP
 * Return true is the mode is correctly switched
 */
bool SIM800L::setPowerMode(PowerMode powerMode) {
  // Check if the power mode requested is not ERROR or UNKNOWN
  if(powerMode == POW_ERROR || powerMode == POW_UNKNOWN) {
    return false;
  }
  
  // Check the current power mode
  PowerMode currentPowerMode = getPowerMode();

  // If the current power mode is undefined, abord
  if(currentPowerMode == POW_ERROR || currentPowerMode == POW_UNKNOWN) {
    return false;
  }

  // If the current power mode is the same that the requested power mode, say it's OK
  if(currentPowerMode == powerMode) {
    return true;
  }
  
  // If SLEEP or MINIMUM, only NORMAL is allowed
  if((currentPowerMode == SLEEP || currentPowerMode == MINIMUM) && (powerMode != NORMAL)) {
    return false;
  }

  // Send the command
  char value;
  switch(powerMode) {
    case MINIMUM : 
      sendCommand_P(AT_CMD_CFUN0);
      break;
    case SLEEP :
      sendCommand_P(AT_CMD_CFUN4);
      break;
    case NORMAL :
    default :
      sendCommand_P(AT_CMD_CFUN1);
  }

  // Read but don't care about the result
  purgeSerial();

  // Check the current power mode
  currentPowerMode = getPowerMode();
  
  // If the current power mode is the same that the requested power mode, say it's OK
  return currentPowerMode == powerMode;
}

/**
 * Status function: Check the strengh of the signal
 */
uint8_t SIM800L::getSignal() {
  sendCommand_P(AT_CMD_CSQ);
  if(readResponse(DEFAULT_TIMEOUT)) {
    int16_t idxBase = strIndex(internalBuffer, "AT+CSQ");
    if(idxBase != 0) {
      return 0;
    }
    int16_t idxEnd = strIndex(internalBuffer, ",", idxBase);
    uint8_t value = internalBuffer[idxEnd - 1] - '0';
    if(internalBuffer[idxEnd - 2] != ' ') {
      value += (internalBuffer[idxEnd - 2] - '0') * 10;
    }
    if(value > 31) {
      return 0;
    }
    return value;
  }
  return 0;
}

/*****************************************************************************************
 * HELPERS
 *****************************************************************************************/
/**
 * Find string "findStr" in another string "str"
 * Returns true if found, false elsewhere
 */
int16_t SIM800L::strIndex(const char* str, const char* findStr, uint16_t startIdx) {
  int16_t firstIndex = -1;
  int16_t sizeMatch = 0;
  for(int16_t i = startIdx; i < strlen(str); i++) {
    if(sizeMatch >= strlen(findStr)) {
      break;
    }
    if(str[i] == findStr[sizeMatch]) {
      if(firstIndex < 0) {
        firstIndex = i;
      }
      sizeMatch++;
    } else {
      firstIndex = -1;
      sizeMatch = 0;
    }
  }

  if(sizeMatch >= strlen(findStr)) {
    return firstIndex;
  } else {
    return -1;
  }
}

/**
 * Init internal buffer
 */
void SIM800L::initInternalBuffer() {
  for(uint16_t i = 0; i < internalBufferSize; i++) {
    internalBuffer[i] = '\0';
  }
}

/**
 * Init recv buffer
 */
void SIM800L::initRecvBuffer() {
  // Cleanup the receive buffer
  for(uint16_t i = 0; i < recvBufferSize; i++) {
    recvBuffer[i] = 0;
  }
}

/*****************************************************************************************
 * LOW LEVEL FUNCTIONS TO COMMUNICATE WITH THE SIM800L MODULE
 *****************************************************************************************/
/**
 * Send AT command to the module
 */
void SIM800L::sendCommand(const char* command) {
  if(enableDebug) {
    debugStream->print(F("SIM800L : Send \""));
    debugStream->print(command);
    debugStream->println(F("\""));
  }
  
  purgeSerial();
  stream->write(command);
  stream->write("\r\n");
  purgeSerial();
}

/**
 * Send AT command coming from the PROGMEM
 */
void SIM800L::sendCommand_P(const char* command) {
  char cmdBuff[32];
  strcpy_P(cmdBuff, command);
  sendCommand(cmdBuff);
}

/**
 * Send AT command to the module with a parameter
 */
void SIM800L::sendCommand(const char* command, const char* parameter) {
  if(enableDebug) {
    debugStream->print(F("SIM800L : Send \""));
    debugStream->print(command);
    debugStream->print(F("\""));
    debugStream->print(parameter);
    debugStream->print(F("\""));
    debugStream->println(F("\""));
  }
  
  purgeSerial();
  stream->write(command);
  stream->write("\"");
  stream->write(parameter);
  stream->write("\"");
  stream->write("\r\n");
  purgeSerial();
}

/**
 * Send AT command coming from the PROGMEM with a parameter
 */
void SIM800L::sendCommand_P(const char* command, const char* parameter) {
  char cmdBuff[32];
  strcpy_P(cmdBuff, command);
  sendCommand(cmdBuff, parameter);
}

/**
 * Purge the serial data
 */
void SIM800L::purgeSerial() {
  stream->flush();
  while (stream->available()) {
    stream->read();
  }
  stream->flush();
}

/**
 * Read from module and expect a specific answer (timeout in millisec)
 */
bool SIM800L::readResponseCheckAnswer_P(uint16_t timeout, const char* expectedAnswer, uint8_t crlfToWait) {"net":{"http_server_properties":{"servers":[{"isolation":[],"server":"https://github.com","supports_spdy":true},{"isolation":[],"server":"https://avatars.githubusercontent.com","supports_spdy":true},{"isolation":[],"server":"https://central.github.com","supports_spdy":true},{"isolation":[],"server":"https://api.github.com","supports_spdy":true},{"alternative_service":[{"advertised_alpns":["h3"],"expiration":"13321412135777832","port":443,"protocol_str":"quic"},{"advertised_alpns":["h3-Q050"],"expiration":"13321412135777833","port":443,"protocol_str":"quic"}],"isolation":[],"network_stats":{"srtt":22736},"server":"https://dns.google","supports_spdy":true}],"supports_quic":{"address":"192.168.10.126","used_quic":true},"version":5},"network_qualities":{"CAESABiAgICA+P////8B":"4G"}}}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        