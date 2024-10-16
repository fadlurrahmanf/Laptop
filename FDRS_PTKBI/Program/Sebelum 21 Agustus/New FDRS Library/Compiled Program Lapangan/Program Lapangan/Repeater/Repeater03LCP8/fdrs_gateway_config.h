//  FARM DATA RELAY SYSTEM
//
//  GATEWAY CONFIGURATION

//Addresses
uint8_t UNIT_MAC = 0x03;  // The address of this gateway

uint8_t ESPNOW_NEIGHBOR_1 = 0x00;  // Address of ESP-NOW neighbor #1
uint8_t ESPNOW_NEIGHBOR_2 = 0x00;  // Address of ESP-NOW neighbor #2
uint8_t LORA_NEIGHBOR_1  =  0x02;  // Address of LoRa neighbor #1
uint8_t LORA_NEIGHBOR_2  =  0x00;  // Address of LoRa neighbor #2

// Interfaces
#define USE_ESPNOW  
#define USE_LORA
//#define USE_WIFI  // Will cause errors if used with ESP-NOW. Use a serial link instead!
//#define USE_ETHERNET

// Routing
// Options: sendESPNowNbr(1 or 2); sendESPNowPeers(); sendLoRaNbr(1 or 2); broadcastLoRa(); sendSerial(); sendMQTT();
#define ESPNOWG_ACT    sendLoRaNbr(1); broadcastLoRa();
#define LORAG_ACT      sendESPNowPeers();
#define SERIAL_ACT     
#define MQTT_ACT          
#define INTERNAL_ACT   sendLoRaNbr(1);
#define ESPNOW1_ACT    
#define ESPNOW2_ACT                    
#define LORA1_ACT      sendLoRaNbr(2); broadcastLoRa(); 
#define LORA2_ACT      sendLoRaNbr(1);

// LoRa Configuration
#define RADIOLIB_MODULE LLCC68
#define LORA_SS    15
#define LORA_RST   16
#define LORA_DIO   5
#define LORA_BUSY  4
#define USE_SX126X

#define LORA_TXPWR 17   // LoRa TX power in dBm (: +2dBm - +17dBm (for SX1276-7) +20dBm (for SX1278))
#define LORA_FREQUENCY     920.0
// #define CUSTOM_SPI
#define LORA_SPI_SCK  14
#define LORA_SPI_MISO 12
#define LORA_SPI_MOSI 13

#define FDRS_DEBUG     // Enable USB-Serial debugging
#define DBG_LEVEL 1    // 0 for minimal messaging, 1 for troubleshooting, 2 for development

// I2C - OLED or RTC
#define I2C_SDA 4
#define I2C_SCL 15

// OLED -- Displays console debugging messages on an SSD1306 I²C OLED
///#define USE_OLED    
#define OLED_HEADER "FDRS"
#define OLED_PAGE_SECS 30
#define OLED_RST 16

// RTC - I2C
// #define USE_RTC_DS3231
// #define RTC_ADDR 0x57
// #define USE_RTC_DS1307
// #define RTC_ADDR 0x68

// UART data interface pins (ESP32 only)
//#define RXD2 14
//#define TXD2 15

//#define USE_LR  // Use ESP-NOW LR mode (ESP32 only)

// WiFi and MQTT Credentials  -- These will override the global settings
//#define WIFI_SSID   "Your SSID"  
//#define WIFI_PASS   "Your Password"

// Use Static IP Address for WiFi connections
// #define USE_STATIC_IPADDRESS     
// #define HOST_IPADDRESS      "192.168.0.100"
// #define GW_IPADDRESS        "192.168.0.1"
// #define SUBNET_ADDRESS      "255.255.255.0"
// #define DNS1_IPADDRESS      "192.168.0.1"    
// #define DNS2_IPADDRESS      "192.168.0.2"  

//#define MQTT_ADDR   "192.168.0.8"
//#define MQTT_PORT   1883 // Default MQTT port is 1883
//#define MQTT_AUTH   //Enable MQTT authentication 
//#define MQTT_USER   "Your MQTT Username"
//#define MQTT_PASS   "Your MQTT Password"

// NTP Time settings
#define USDST
// #define EUDST
#define TIME_SERVER       "0.us.pool.ntp.org"       // NTP time server to use. If FQDN at least one DNS server is required to resolve name
#define STD_OFFSET      (-6)                // Local standard time offset in hours from UTC - if unsure, check https://time.is
#define DST_OFFSET      (STD_OFFSET + 1)    // Local savings time offset in hours from UTC - if unsure, check https://time.is
#define TIME_FETCHNTP     60      // Time, in minutes, between fetching time from NTP server
#define TIME_PRINTTIME    15     // Time, in minutes, between printing local time to debug
#define TIME_SEND_INTERVAL 10    // Time, in minutes, between sending out time to remote devices