// wifi connection data
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include "DHT.h"

// lcd data
LiquidCrystal_I2C lcd(0x27,16,2);

// button data
unsigned long lastReminderTime = 0;
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
const unsigned long reminderInterval = 60000;
int lastButtonState = 0;   // the previous reading from the input pin
int remainingDoses = 100;
const int buttonPin = D6;
const int vibPin = D5;

// led data
const int ledPin = D0;

// gps data
char datoCmd = 0;
TinyGPSPlus gps;
SoftwareSerial ss(D3, D4);

// dht pin data
#define DPIN D7
#define DTYPE DHT11
DHT dht(DPIN,DTYPE);

// // wifi connection data
// const char* ssid = "Sofi-IoT";
// const char* password = "password";
// HTTPClient http;
// WiFiClientSecure wifiClient;

void sendGPSData() {
  if (gps.location.isValid()) {
    Serial.println("Longitud:");
    Serial.println(gps.location.lng(),6);
    Serial.println("Latitude:");
    Serial.println(gps.location.lat(),6);
  
  //   String sendGPS = "https://apex.oracle.com/pls/apex/proyectoiot/senddata/sendgps";
  //   sendGPS += "?latitude=";
  //   sendGPS += String(gps.location.lng(), 6); // Latitude with 6 decimal places
  //   sendGPS += "&longitude=";
  //   sendGPS += String(gps.location.lat(), 6); // Longitude with 6 decimal places

  //   http.begin(wifiClient, sendGPS);
  //   int httpCode = http.GET();
  //   Serial.println(httpCode);
  //   if (httpCode > 0) {
  //     // Check for the returning status code
  //     if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_CREATED) {
  //         String response = http.getString();
  //         Serial.println("Server response: " + response);
  //         Serial.println("GPS data sent!");
  //     } else {
  //       Serial.println("Error code: " + String(httpCode));
  //     }
  //   }
  //   http.end();
  // } else {
  //   Serial.println("Invalid GPS Data");
  // }
  }
}

void sendReminder(){
  for (int i = 0; i < 3; i++) { // Repeat 3 times
        digitalWrite(vibPin, HIGH); // Turn on the vibration motor
        delay(1500); // Keep it on for 3 seconds (3000 milliseconds)
        digitalWrite(vibPin, LOW); // Turn off the motor
        if (i < 2) { // To avoid delay after the last vibration
            delay(1000); // Wait for 1 second (1000 milliseconds) before repeating
        }
  }

  // String sendReminder = "https://apex.oracle.com/pls/apex/proyectoiot/senddata/sendreminder";

  // http.begin(wifiClient,sendReminder);
  // int httpCode = http.GET();
  // Serial.println(httpCode);
  // if (httpCode > 0) {
  //   // Check for the returning status code
  //   if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_CREATED) {
  //       // Parse and process the response if needed
  //       String response = http.getString();
  //       Serial.println("Server response: " + response);
  //       Serial.println("Reminder registered!");
  //   } 
  //   else {
  //     Serial.println("Error code: " + String(httpCode));
  //   }
  // }
  // http.end();
}

void sendUse(){
  // String sendUse = "https://apex.oracle.com/pls/apex/proyectoiot/senddata/senduses";
  // sendUse += "?remaining=";
  // sendUse += String(remainingDoses);

  // http.begin(wifiClient,sendUse);
  // int httpCode = http.GET();
  // Serial.println(httpCode);
  // if (httpCode > 0) {
  //   // Check for the returning status code
  //   if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_CREATED) {
  //       // Parse and process the response if needed
  //       String response = http.getString();
  //       Serial.println("Server response: " + response);
  //       Serial.println("Use registered!");
  //   } 
  //   else {
  //     Serial.println("Error code: " + String(httpCode));
  //   }
  // }
  // http.end();
}

void sendDHTData() {
  float temperature = dht.readTemperature(false);
  float hum = dht.readHumidity(false);
  String status = getStatus(temperature, hum);

  if (isnan(temperature) || isnan(hum)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Temperature: ");
  Serial.println(temperature);
  Serial.print("Humidity: ");
  Serial.println(hum);

  // String sendDHT = "https://apex.oracle.com/pls/apex/proyectoiot/senddata/sendtemp";
  // sendDHT += "?temp=";
  // sendDHT += String(temperature);
  // sendDHT += "&humedad=";
  // sendDHT += String(humidity);
  // sendDHT += "&status=";
  // sendDHT += status;

  // http.begin(wifiClient, sendDHT);
  // int httpCode = http.GET();

  // if (httpCode > 0) {
  //   if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_CREATED) {
  //       String response = http.getString();
  //       Serial.println("Server response: " + response);
  //       Serial.println("DHT data sent!");
  //   } else {
  //       Serial.println("Error sending DHT data: " + String(httpCode));
  //   }
  // } else {
  //   Serial.println("Error in HTTP request");
  // }

  // http.end();
}

String getStatus(float temp,float hum){
  if (temp > 30 || hum > 70) {
        return "malo";
    } else if (hum > 50) {
        return "ok";
    } else {
        return "buena";
    }
}

void setup() { 
  Serial.begin(115200);
  ss.begin(9600);

  // button setup
  pinMode(buttonPin, INPUT);

  // vib pin setup
  pinMode(vibPin, OUTPUT);

  // led pin setup
  pinMode(ledPin, OUTPUT);

  // WiFi.begin(ssid, password);// initialize the WiFi connection
  // // checking for unsuccessful connection
  // Serial.println("Connecting");
  // while(WiFi.status() != WL_CONNECTED){
  //   delay(500);
  //   Serial.print(".");
  // }
  
  // // continue after successful connection
  // Serial.println("");
  // Serial.println("WiFi connected");

  digitalWrite(ledPin,HIGH);

  // lcd setup
  lcd.init();
  lcd.backlight();
  lcd.print("Dosis res: ");
  lcd.print(remainingDoses);
}



void loop() {
  unsigned long currentMillis = millis();
  int reading = digitalRead(buttonPin);
  delay(500);
  Serial.println(reading);

  // Debounce logic
  if (reading > lastButtonState && currentMillis - lastDebounceTime > debounceDelay) {
    lastDebounceTime = currentMillis;
    
    if (remainingDoses > 0) {
        remainingDoses--;
        lcd.clear();
        lcd.print("Dosis res: ");
        lcd.print(remainingDoses);
    }
  }
  // Reminder logic
  if (currentMillis - lastReminderTime >= reminderInterval) {
    sendReminder();
    lastReminderTime = currentMillis;
  }
  lastButtonState = reading;

  // gps logic
  while (ss.available()) {
    char c = ss.read();
    gps.encode(c);
  }
  sendGPSData();

  // dht logic
  sendDHTData();
}
