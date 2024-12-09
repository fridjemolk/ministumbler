#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <WiFiGeneric.h>
#include <WiFiMulti.h>
#include <WiFiScan.h>
#include <WiFiServer.h>
#include <WiFiSTA.h>
#include <WiFiType.h>
#include <WiFiUdp.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

/*
Includes some code inspired by and modified from: https://github.com/dkyazzentwatwa/esp32-gps-wifi-wigle/blob/main/esp32-gps-wifi-wigle.ino
*/
static const int RXPin = D7, TXPin = D6;
static const uint32_t GPSBaud = 9600;

// The TinyGPSPlus object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

// Counters for WiFi scans and networks found
int scanCount = 0;
int networkCount = 0;
int gpsPrints = 0;

void setup()
{
  Serial.begin(115200);
  ss.begin(GPSBaud);

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  //Serial.println(F("DeviceExample.ino"));
  //Serial.println(F("A simple demonstration of TinyGPSPlus with an attached GPS module"));
  //Serial.print(F("Testing TinyGPSPlus library v. ")); Serial.println(TinyGPSPlus::libraryVersion());
  //Serial.println(F("by Mikal Hart"));
  //Serial.println();

  Serial.print("Latitude");
  Serial.print(F(","));
  Serial.print("Longitude");
  Serial.print(F(","));
  Serial.print("Month");
  Serial.print(F(","));
  Serial.print("Day");
  Serial.print(F(","));
  Serial.print("Year");
  Serial.print(F(","));
  Serial.print("Hour");
  Serial.print(F(","));
  Serial.print("Minute");
  Serial.print(F(","));
  Serial.print("Second");
  Serial.print(F(","));
  Serial.print("Centisecond");
  Serial.print(F(","));
  Serial.print("Scan Count");
  Serial.print(F(","));
  Serial.print("Network Count");
  Serial.print(F(","));
  Serial.print("SSID");
  Serial.print(F(","));
  Serial.print("RSSI");
  Serial.print(F(","));
  Serial.print("MAC");
  Serial.print(F(","));
  Serial.println("");

}

void loop(){
  // This sketch displays information every time a new sentence is correctly encoded.
  while (ss.available() > 0){
    if (gps.encode(ss.read()) && gps.location.isValid() && gps.date.isValid() && gps.time.isValid()){
      scanWiFiNetworks();
      digitalWrite(LED_BUILTIN, HIGH);
    } else {
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
}


void printGPS(){
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
    Serial.print(F(","));
  }

  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F(","));
    Serial.print(gps.date.day());
    Serial.print(F(","));
    Serial.print(gps.date.year());
    Serial.print(F(","));
  }
  
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(","));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(","));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F(","));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  Serial.print(F(","));
}

// Scan for WiFi networks and store results in the CSV file
void scanWiFiNetworks() {
  printGPS();
  networkCount = WiFi.scanNetworks(); // Scan for WiFi networks
  scanCount++; // Increment scan count
  
  // Print networks to serial monitor
  Serial.print(scanCount);
  Serial.print(F(","));
  Serial.print(networkCount);
  Serial.print(F(","));
  for (int i = 0; i < networkCount; ++i) {
    Serial.print(WiFi.SSID(i));
    Serial.print(F(","));
    Serial.print(WiFi.RSSI(i));
    Serial.print(F(","));
    //Serial.print(WiFi.macAddress());
  }
  Serial.println("");
}
