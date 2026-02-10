#include <WiFi.h>
#include <SPI.h>
#include <HardwareSerial.h>
#include <TinyGPS++.h>
#include <SD.h>

//TinyGPS++ library documentation is archived here from the developer's now-dead wordpress: https://web.archive.org/web/20250805002451/https://arduiniana.org/libraries/tinygpsplus/
//Quectel L76K GPS Module Protocol Documentation is available here: https://raw.githubusercontent.com/Seeed-Projects/Seeed_L76K-GNSS_for_XIAO/fb74b715224e0ac153c3884e578ee8e024ed8946/docs/Quectel_L76K_GNSS_Protocol_Specification_V1.1.pdf

#define SD_FAT_TYPE 3
#define SD_CS_PIN A1

File csv;
/*
This firmware is inspired by and modified from: 
https://github.com/lozaning/The_Wifydra/blob/main/Dom.ino
https://github.com/lozaning/The_Wifydra/blob/main/Sub.ino

This sketch draws HEAVILY from the above two sources, essentially just modifying them to work as a single script. Credit to https://github.com/lozaning for all of his featured code, modified or unmodified.

https://github.com/dkyazzentwatwa/esp32-gps-wifi-wigle/blob/main/esp32-gps-wifi-wigle.ino
*/
static const int RXPin = D7, TXPin = D6;
static const uint32_t GPSBaud = 9600; // GPS module default baud rate is 9600

//static const bool ENABLE_LED = false;

// The TinyGPSPlus object
TinyGPSPlus gps;

// The serial connection to the GPS device
HardwareSerial GPSSerial(1);

// Counters for WiFi scans and networks found
int scanCount = 0;
int networkCount = 0;
int gpsPrints = 0;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct struct_message {
  char bssid[64];
  char ssid[32];
  char encryptionType[16];
  int32_t channel;
  int32_t rssi;
} struct_message;

// Constants defined here:
#define LOG_FILE_PREFIX "/gpslog"
#define MAX_LOG_FILES 100
#define LOG_FILE_SUFFIX "csv"
#define LOG_COLUMN_COUNT 11
#define LOG_RATE 500
#define NOTE_DH2 661
const int chipSelect = 10;
File myFile;
char logFileName[13];
int totalNetworks = 0;
unsigned long lastLog = 0;
int integerAltMeters = 0;

//say how many macs we should keep in the buffer to compare for uniqueness
#define mac_history_len 512

const String wigleHeaderFileFormat = "WigleWifi-1.4,appRelease=2.26,model=Ministumbler v0.2,release=0.0.1,device=Ministumbler,display=3fea5e7,board=esp32,brand=Fridje";
char* log_col_names[LOG_COLUMN_COUNT] = {
  "MAC", "SSID", "AuthMode", "FirstSeen", "Channel", "RSSI", "CurrentLatitude", "CurrentLongitude", "AltitudeMeters", "AccuracyMeters", "Type"
};

struct mac_addr {
  unsigned char bytes[6];
};
struct mac_addr mac_history[mac_history_len];
unsigned int mac_history_cursor = 0;

String AP;
String BSSIDchar;
String ENC;
String EncTy;
// Create a struct_message called myData
struct_message myData;

unsigned long lastTime = 0;
unsigned long timerDelay = 200;  // send readings timer

String recentSSID;
String recentSSID1;
String recentSSID2;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin(115200);
  if(!Serial){
    delay(1);
  } else {
    Serial.println("Serial initialized");
  }
  GPSSerial.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin);
  if(!GPSSerial){
    delay(1);
  } else {
    Serial.println("GPS initialized");
  }
  delay(50);
  Serial.println("Telling L76K which satellite mode to use...");
  Serial.println();
  delay(50);
  GPS_Serial.print('$PCAS04,7*1E'); //Tell L76K to use GPS + BeiDou +  GLONASS, which is all the satellite types it supports + QZSS which is always enabled. This will give the best chance at a fix.
  GPS_Serial.print('\r\n');
  //Calculate checksum with https://www.meme.au/nmea-checksum.html
  delay(250);
  
  Serial.print("Initializing SD card...");

  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("initialization failed! Check if a card is inserted, and if not, check solder joints on SD module");
    while(true){
      delay(1);
    }
  }
  Serial.println("SD and GPS initialization done.");

  updateFileName();
  printHeader();

  // initialize digital pin LED_BUILTIN as an output.
  //pinMode(LED_BUILTIN, OUTPUT);
  //digitalWrite(LED_BUILTIN, HIGH);

}

void loop() {
  //Check for GPS lock
  smartDelay(50);
  if (gps.location.isValid() && gps.date.isValid() && gps.time.isValid() && (gps.time.hour() > 0 || gps.time.minute() > 0 || gps.time.second() > 0) && gps.date.year() > 2000){
    print_GPS_date_time();
    char Buf[50];
    char bufBSSID[64];
    char BufEnc[50];
    if ((millis() - lastTime) > timerDelay) {
      // Set values to send

      //myData.b = random(1,20);
      //myData.c = 1.2;
      int n = WiFi.scanNetworks();
      if (n == 0) {
        Serial.println("No networks found");
        Serial.println("No networks found");
      } else {
        for (int8_t i = 0; i < n; i++) {
          //delay(10);
          if (seen_mac(WiFi.BSSID(i))) {
            Serial.println("We've already seen it");
            //BSSIDchar = WiFi.BSSID(i);
            //BSSIDchar.toCharArray(bufBSSID, 64);
            //strcpy(myData.bssid, Buf);
            //Serial.println(myData.bssid);
            continue;
          }
          Serial.println("We havent seen it");
          String MacString = WiFi.BSSIDstr(i).c_str();
          //myData.bssid = MacString;
          MacString.toCharArray(bufBSSID, 64);
          strcpy(myData.bssid, bufBSSID);
          Serial.println(myData.bssid);
          //myData.bssid = WiFi.BSSID(i);
          //Serial.print("MyData.bssid: ");

          //Serial.println(myData.bssid);
          String AP = WiFi.SSID(i);
          AP.toCharArray(Buf, 50);
          strcpy(myData.ssid, Buf);
          Serial.print("SSID: ");
          Serial.println(myData.ssid);
          //String ENC = security_int_to_string(WiFi.encryptionType(i));

          //ENC.toCharArray(BufEnc, 32);
          //strcpy(myData.encryptionType, BufEnc);
          //myData.encryptionType = authtype;
          switch (WiFi.encryptionType(i)) {
            case WIFI_AUTH_OPEN:
              EncTy = "Open";
              break;
            case WIFI_AUTH_WEP:
              EncTy = "WEP";
              break;
            case WIFI_AUTH_WPA_PSK:
              EncTy = "WPA PSK";
              break;
            case WIFI_AUTH_WPA2_PSK:
              EncTy = "WPA2 PSK";
              break;
            case WIFI_AUTH_WPA_WPA2_PSK:
              EncTy = "WPA/WPA2 PSK";
              break;
            case WIFI_AUTH_WPA2_ENTERPRISE:
              EncTy = "WPA2 Enterprise";
              break;
            default:
              EncTy = "Unknown";
              break;
          }
          EncTy.toCharArray(BufEnc, 16);
          strcpy(myData.encryptionType, BufEnc);
          Serial.print("Encryption: ");
          Serial.println(myData.encryptionType);

          myData.channel = WiFi.channel(i);
          myData.rssi = WiFi.RSSI(i);
          save_mac(WiFi.BSSID(i));
          writeToCSV((uint8_t*)&myData, sizeof(myData));
          //digitalWrite(2, LOW);
          delay(200);
          //digitalWrite(2, HIGH);
        }
        lastTime = millis();
      }
    }
  } else {
    Serial.println("Waiting for GPS Lock...");
    while (GPSSerial.available())
      gps.encode(GPSSerial.read());
    Serial.print("Current garbage GPS time: ");
    Serial.print(gps.date.year());
    Serial.print(F("-"));
    Serial.print(gps.date.month());
    Serial.print(F("-"));
    Serial.print(gps.date.day());
    Serial.print(F(" "));
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F(":"));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
    Serial.println(".");
    smartDelay(1000);
  }
}

void print_GPS_date_time(){
    Serial.print("Current GPS time: ");
    Serial.print(gps.date.year());
    Serial.print(F("-"));
    Serial.print(gps.date.month());
    Serial.print(F("-"));
    Serial.print(gps.date.day());
    Serial.print(F(" "));
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F(":"));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
    Serial.println(".");
}

void save_mac(unsigned char* mac) {
  //Save a MAC address into the recently seen array.
  if (mac_history_cursor >= mac_history_len) {
    mac_history_cursor = 0;
  }
  struct mac_addr tmp;
  for (int x = 0; x < 6; x++) {
    tmp.bytes[x] = mac[x];
  }

  mac_history[mac_history_cursor] = tmp;
  mac_history_cursor++;
  Serial.print("Mac len ");
  Serial.println(mac_history_cursor);
}

bool mac_cmp(struct mac_addr addr1, struct mac_addr addr2) {
  //Return true if 2 mac_addr structs are equal.
  for (int y = 0; y < 6; y++) {
    if (addr1.bytes[y] != addr2.bytes[y]) {
      return false;
    }
  }
  return true;
}

bool seen_mac(unsigned char* mac) {
  //Return true if this MAC address is in the recently seen array.

  struct mac_addr tmp;
  for (int x = 0; x < 6; x++) {
    tmp.bytes[x] = mac[x];
  }

  for (int x = 0; x < mac_history_len; x++) {
    if (mac_cmp(tmp, mac_history[x])) {
      return true;
    }
  }
  return false;
}

void print_mac(struct mac_addr mac) {
  //Print a mac_addr struct nicely.
  for (int x = 0; x < 6; x++) {
    Serial.print(mac.bytes[x], HEX);
    Serial.print(":");
  }
}

String security_int_to_string(int security_type) {
  //Provide a security type int from WiFi.encryptionType(i) to convert it to a String which Wigle CSV expects.
  String authtype = "";
  switch (security_type) {
    case WIFI_AUTH_OPEN:
      authtype = "[OPEN]";
      break;

    case WIFI_AUTH_WEP:
      authtype = "[WEP]";
      break;

    case WIFI_AUTH_WPA_PSK:
      authtype = "[WPA_PSK]";
      break;

    case WIFI_AUTH_WPA2_PSK:
      authtype = "[WPA2_PSK]";
      break;

    case WIFI_AUTH_WPA_WPA2_PSK:
      authtype = "[WPA_WPA2_PSK]";
      break;

    case WIFI_AUTH_WPA2_ENTERPRISE:
      authtype = "[WPA2]";
      break;

    //Requires at least v2.0.0 of https://github.com/espressif/arduino-esp32/
    case WIFI_AUTH_WPA3_PSK:
      authtype = "[WPA3_PSK]";
      break;

    case WIFI_AUTH_WPA2_WPA3_PSK:
      authtype = "[WPA2_WPA3_PSK]";

    default:
      authtype = "";
  }

  return authtype;
}

void updateFileName() {
  int i = 0;
  for (; i < MAX_LOG_FILES; i++) {
    memset(logFileName, 0, strlen(logFileName));
    sprintf(logFileName, "%s%d.%s", LOG_FILE_PREFIX, i, LOG_FILE_SUFFIX);
    if (!SD.exists(logFileName)) {
      Serial.println("we picked a new file name");
      Serial.println(logFileName);
      break;
    } else {
      Serial.print(logFileName);
      Serial.println(" exists");
    }
  }
  Serial.println("File name: ");
  Serial.println(logFileName);
}

void printHeader() {
  File logFile = SD.open(logFileName, FILE_WRITE);
  if (logFile) {
    int i = 0;
    logFile.println(wigleHeaderFileFormat);  // comment out to disable Wigle header
    for (; i < LOG_COLUMN_COUNT; i++) {
      logFile.print(log_col_names[i]);
      if (i < LOG_COLUMN_COUNT - 1)
        logFile.print(',');
      else
        logFile.println();
    }
    logFile.close();
  }
}

static void smartDelay(unsigned long ms)  // custom version of delay() ensures that the gps object is being "fed".
{
  unsigned long start = millis();
  do {
    while (GPSSerial.available())
      gps.encode(GPSSerial.read());
  } while (millis() - start < ms);
}

//Function that will be executed when scan is complete
void writeToCSV(const uint8_t* incomingData, int len) {
  File logFile = SD.open(logFileName, FILE_APPEND);
  memcpy(&myData, incomingData, sizeof(myData));
  char buffer[10];
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Mac: ");
  Serial.println(myData.bssid);
  logFile.print(myData.bssid);
  logFile.print(",");
  Serial.print("SSID: ");
  Serial.println(myData.ssid);
  String SSIDString = myData.ssid;
  if(SSIDString.indexOf(",") > 0) {
    SSIDString = "\""+SSIDString+"\"";
  }
  logFile.print(SSIDString);
  logFile.print(",");
  Serial.print("Encryption: ");
  Serial.println(myData.encryptionType);
  logFile.print(myData.encryptionType);
  logFile.print(",");
  Serial.print("First seen: ");

  //date
  Serial.println(gps.date.year());
  logFile.print(gps.date.year());
  logFile.print("-");

  snprintf(buffer , sizeof buffer, "%02d", gps.date.month());
  Serial.print(buffer);
  logFile.print(buffer);
  logFile.print("-");

  snprintf(buffer , sizeof buffer, "%02d", gps.date.day());
  Serial.print(buffer);
  logFile.print(buffer);

  logFile.print(" ");

  //time
  snprintf(buffer , sizeof buffer, "%02d", gps.time.hour());
  Serial.print(buffer);
  logFile.print(buffer);
  logFile.print(":");

  snprintf(buffer , sizeof buffer, "%02d", gps.time.minute());
  Serial.print(buffer);
  logFile.print(buffer);
  logFile.print(":");

  snprintf(buffer , sizeof buffer, "%02d", gps.time.second());
  Serial.print(buffer);
  logFile.print(buffer);

  logFile.print(",");
  Serial.print("Channel: ");
  Serial.println(myData.channel);
  logFile.print(myData.channel);
  logFile.print(",");
  Serial.print("RSSI:  ");
  Serial.println(myData.rssi);
  logFile.print(myData.rssi);
  logFile.print(",");
  Serial.print("Lat: ");
  Serial.println(gps.location.lat(), 8);
  logFile.print(gps.location.lat(), 8);
  logFile.print(",");
  Serial.print("Lon: ");
  Serial.println(gps.location.lng(), 8);
  logFile.print(gps.location.lng(), 8);
  logFile.print(",");
  Serial.print("Altitude: ");
  integerAltMeters = int(round(gps.altitude.meters()));
  Serial.println(integerAltMeters);
  logFile.print(integerAltMeters);
  logFile.print(",");
  Serial.print("HDOP: ");
  Serial.println(gps.hdop.value());
  // Convert HDOP to accuracy in meters (HDOP*100 / 10 = rough accuracy)
  logFile.print(gps.hdop.value() / 10.0, 1);
  logFile.print(",");
  Serial.print("BiD: ");
  logFile.print("WIFI");
  logFile.println();
  logFile.close();
  recentSSID2 = recentSSID1;
  recentSSID1 = recentSSID;
  recentSSID = myData.ssid;
  Serial.println();
  totalNetworks +=1;
  Serial.print("Total Networks: ");
  Serial.println(totalNetworks);

  Serial.println();
  Serial.println();
  Serial.println();

}




