#include <SoftwareSerial.h>
#include <TinyGsmClient.h>
#include <Adafruit_GPS.h>

#define GSM_RX_PIN 7
#define GSM_TX_PIN 8
#define GPS_RX_PIN 9
#define GPS_TX_PIN 10


SoftwareSerial gsmSerial(GSM_TX_PIN, GSM_RX_PIN);
TinyGsm modem(gsmSerial);
Adafruit_GPS GPS(&Serial1);

const char apn[]  = "your_apn"; // Set your APN here
const char user[] = ""; // Set APN username if required
const char pass[] = ""; // Set APN password if required

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  gsmSerial.begin(9600);

  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  
  // Initialize GSM module
  Serial.println("Initializing modem...");
  modem.restart();
  modem.gprsConnect(apn, user, pass);
}

void loop() {
  GPS.read();
  if (GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA())) return;
  }
  
  if (GPS.fix) {
    float latitude = GPS.latitudeDegrees;
    float longitude = GPS.longitudeDegrees;
    
    if (modem.isGprsConnected()) {
      sendGPSData(latitude, longitude);
    }
  }
  delay(10000); // Wait for 10 seconds before sending next data
}

void sendGPSData(float lat, float lon) {
  TinyGsmClient client(modem);
  const char* host = "script.google.com";
  const int port = 80;
  
  if (!client.connect(host, port)) {
    Serial.println("Connection failed");
    return;
  }

  String url = " "//your google apps script code of the spreadsheet
  url += "?latitude=";
  url += String(lat, 6);
  url += "&longitude=";
  url += String(lon, 6);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  unsigned long timeout = millis();
  while (client.connected() && millis() - timeout < 10000L) {
    while (client.available()) {
      char c = client.read();
      Serial.print(c);
      timeout = millis();
    }
  }
  client.stop();
  Serial.println("\nGPS data sent");
}
