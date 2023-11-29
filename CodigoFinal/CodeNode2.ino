#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);

unsigned long lastReminderTime = 0;
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
const unsigned long reminderInterval = 20000;
int lastButtonState = 0;   // the previous reading from the input pin
int remainingDoses = 100;
const int buttonPin = D6;
const int vibPin = 14;

// wifi connection data
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>


const char* ssid = "Sofi-IoT";
const char* password = "password";
HTTPClient http;
WiFiClientSecure wifiClient;



void sendReminder(){
  for (int i = 0; i < 3; i++) { // Repeat 3 times
        digitalWrite(vibPin, HIGH); // Turn on the vibration motor
        delay(1500); // Keep it on for 3 seconds (3000 milliseconds)
        digitalWrite(vibPin, LOW); // Turn off the motor
        if (i < 2) { // To avoid delay after the last vibration
            delay(1000); // Wait for 1 second (1000 milliseconds) before repeating
        }
  }

  String sendReminder = "https://apex.oracle.com/pls/apex/proyectoiot/senddata/sendreminder";

  http.begin(wifiClient,sendReminder);
  int httpCode = http.GET();
  Serial.println(httpCode);
  if (httpCode > 0) {
    // Check for the returning status code
    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_CREATED) {
        // Parse and process the response if needed
        String response = http.getString();
        Serial.println("Server response: " + response);
        Serial.println("Reminder registered!");
    } 
    else {
      Serial.println("Error code: " + String(httpCode));
    }
  }
  http.end();
}

void sendUse(){
  String sendUse = "https://apex.oracle.com/pls/apex/proyectoiot/senddata/senduses";
  sendUse += "?remaining=";
  sendUse += String(remainingDoses);

  http.begin(wifiClient,sendUse);
  int httpCode = http.GET();
  Serial.println(httpCode);
  if (httpCode > 0) {
    // Check for the returning status code
    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_CREATED) {
        // Parse and process the response if needed
        String response = http.getString();
        Serial.println("Server response: " + response);
        Serial.println("Use registered!");
    } 
    else {
      Serial.println("Error code: " + String(httpCode));
    }
  }
  http.end();
}

void setup() { 
  Serial.begin(115200);

  // button setup
  pinMode(buttonPin, INPUT);

  // vib pin setup
  pinMode(vibPin, OUTPUT);

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
}
