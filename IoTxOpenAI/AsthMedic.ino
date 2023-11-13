// Brenda Sofia Sandoval A00838209

#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h> // Include the library for the I2C LCD
#include <WiFi.h>
#include "DHTesp.h"

const char* ssid = "Wokwi-GUEST";
const char* password = "";
const int DHT_PIN = 22;
DHTesp dhtSensor;
float previousTemperature = 0; // Update the previous temperature reading
LiquidCrystal_I2C lcd(0x27, 16, 2);


void postToAPI(float temp, float hum){
  DynamicJsonDocument doc(1024);
  
  // Construct the content
  String content;
  content.reserve(200); // Prevents frequent reallocation
  content += "Pretend you're a doctor with a specialty in asthma. I will be providing you with the following data (Respond in a format that will be printed on a 16x2 lcd):\\n";
  content += "Temperature: ";
  content += temp;
  content += "°C\\n"; // Escaping backslashes
  content += "Humidity: ";
  content += hum;
  content += "%\\n";
  content += "Please respond GOOD conditions :D or BAD conditions :( if conditions are suitable or not suitable for asthma patients based on this data. Do not reprint data given.";

  // Populate the JSON Document
  doc["model"] = "gpt-3.5-turbo";
  JsonArray messages = doc.createNestedArray("messages");

  JsonObject message1 = messages.createNestedObject();
  message1["role"] = "system";
  message1["content"] = "You are a doctor with a specialty in asthma.";

  JsonObject message2 = messages.createNestedObject();
  message2["role"] = "user";
  message2["content"] = content;

  // Serialize JSON to String
  String jsonPayload;
  serializeJson(doc, jsonPayload);

  Serial.println(jsonPayload);

  HTTPClient http;
  http.begin("https://api.openai.com/v1/chat/completions");
  http.addHeader("Authorization", "Bearer API_KEY_AQUIII");  // Reemplaza con tu clave de API de OpenAI
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(jsonPayload);
  String payload = http.getString();
  Serial.println("HTTP Response: " + payload);

   if (httpCode > 0) {
    DynamicJsonDocument doc2(1024); // Adjust size as necessary
    deserializeJson(doc2, payload);
    String answer = doc2["choices"][0]["message"]["content"].as<String>();

    // Print the question and answer in the terminal
    Serial.println("P: Are the current temperature (" + String(temp) + "°C) and humidity (" + String(hum) + "%) conditions suitable for asthma patients?");
    Serial.println("R: " + answer);
    lcd.clear();
    lcd.print(answer);
  } else {
    Serial.print("Error on HTTP request, status code: ");
    Serial.println(httpCode);
  }
  http.end();
}

void displayOnLCD(const String &text) {
  lcd.clear(); // Clear any previous text

  // If the text is less than or equal to 16 characters, display it on the first line.
  if (text.length() <= 16) {
    lcd.setCursor(0, 0); // Set cursor to the first line
    lcd.print(text);
  }
  // If the text is longer than 16 characters but less than or equal to 32 characters
  else if (text.length() <= 32) {
    lcd.setCursor(0, 0); // Set cursor to the first line
    lcd.print(text.substring(0, 16)); // Print the first 16 characters
    lcd.setCursor(0, 1); // Set cursor to the second line
    lcd.print(text.substring(16)); // Print the rest of the text
  }
  // If the text is longer than 32 characters, implement scrolling or truncate the text
  else {
    // Implement scrolling or truncate as per your requirement
    // This example truncates the text for simplicity
    lcd.setCursor(0, 0); // Set cursor to the first line
    lcd.print(text.substring(0, 16)); // Print the first 16 characters
    lcd.setCursor(0, 1); // Set cursor to the second line
    lcd.print(text.substring(16, 32)); // Print the next 16 characters
  }
}



void setup() {
  lcd.init();
  lcd.backlight(); // Turn on the backlight
  lcd.setCursor(0, 0); // Set the cursor to the top-left corner


  Serial.begin(9600);
  Serial.println("Counter\n\n");

  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  lcd.print("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  delay(100);
}

void loop() {
  float temperature = dhtSensor.getTemperature();
  float humidity = dhtSensor.getHumidity();
  Serial.println(temperature);
  Serial.println(humidity);

  if(temperature != previousTemperature){
    postToAPI(temperature,humidity);
  }
  
  previousTemperature = temperature; // Update the previous temperature reading

  delay(5000); // this speeds up the simulation
  
}

