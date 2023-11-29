#include <TinyGPS++.h>
#include <SoftwareSerial.h> 
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

// data sensor gps
int RXPin = 3;
int TXPin = 1;
SoftwareSerial gpsSerial(RXPin,TXPin);
TinyGPSPlus gps;
String latitude = "";
String longitude = "";

// data temp and hum sens
#define DPIN D2
#define DTYPE DHT11
DHT dht(DPIN,DTYPE);
float prevTemp = 0;
float temp = 0;
float prevHum = 0;
float hum = 0;

// define led 
int ledPin = D1;


// wifi connection data
const char* ssid = "Sofi-IoT";
const char* password = "password";
const int watchdog = 5000;
unsigned long previousMillis = millis();
WiFiClientSecure wifiClient;
HTTPClient http;
// methods -----------------------------------------------------------------------------------------

void readNSendGPS(){
  if(gpsSerial.available() > 0){
    if(gps.encode(gpsSerial.read())){
      if(gps.location.isUpdated()){
        latitude = gps.location.lat(); // variables that will store the data needed
        longitude = gps.location.lng();

        Serial.print("Latitude: ");
        Serial.println(latitude);
        Serial.print("Longitude: ");
        Serial.println(longitude);

        String sendGPS = "https://apex.oracle.com/pls/apex/proyectoiot/senddata/sendgps";
        sendGPS += "?latitude=";
        sendGPS += latitude;
        sendGPS += "&longitude=";
        sendGPS += longitude;

        http.begin(wifiClient,sendGPS);
        int httpCode = http.GET();
        if (httpCode > 0) {
          // Check for the returning status code
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_CREATED) {
              // Parse and process the response if needed
              String response = http.getString();
              Serial.println("Server response: " + response);
              Serial.println("Data succesfully inserted!");
          } 
          else {
            Serial.println("Error code: " + String(httpCode));
          }
        }
        http.end();
      }
    }
  }
}

void readNSendDHT(){
  temp = dht.readTemperature(false);
  hum = dht.readHumidity();
  if(prevTemp != temp && prevHum != hum){
    String sendDHT = "https://apex.oracle.com/pls/apex/proyectoiot/senddata/sendtemp";
    sendDHT += "?temp=";
    sendDHT += temp;
    sendDHT += "&humedad=";
    sendDHT += hum;

    http.begin(wifiClient,sendDHT);
    int httpCode = http.GET();
    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_CREATED) {    
        String response = http.getString();
        Serial.println("Server response: " + response);
        Serial.println("Data succesfully inserted!");
      } 
      else {
        Serial.println("Error code: " + String(httpCode));
      }
    }
  }
}


// program -----------------------------------------------------------------------------------------

void setup(){
  Serial.begin(9600);

  // setup gps
    gpsSerial.begin(9600);
    Serial.println("GPS sensor initialized");

  // set up led
    pinMode(ledPin,INPUT);

  // setup wifi
    WiFi.begin(ssid, password);// initialize the WiFi connection
    // checking for unsuccessful connection
    Serial.println("Connecting");
    while(WiFi.status() != WL_CONNECTED){
      delay(500);
      Serial.print(".");
  }
  
  // continue after successful connection
  Serial.println("");
  Serial.println("WiFi connected");
  digitalWrite(ledPin,1);
}



void loop(){
  readNSendDHT(); // temp and humidity data are sent if they are unique
  readNSendGPS(); // gps data is sent 
  delay(10000);
}