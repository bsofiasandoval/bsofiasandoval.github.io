#include <ESP8266WiFi.h>

const int pot = A0;    // Pin analógico para el potenciómetro
const int LEDPin = D0;     // Pin para el control del LED

// datos para la conexion a internet
const char* ssid = "Dani";// SSID, the name of your WiFi connection
const char* password = "dztv8752";// password of the WiFi
const char* host = "192.168.114.145";// IP server, obtained with 'ipconfig' in command shell
const int port = 80;// server port, configured in xampp for Apache server, default is: 80

const int watchdog = 5000;// watchdog frequency, reconnect timeout
unsigned long previousMillis = millis();
int c = 0;// the data


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Counter\n\n");
  
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
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(pot,INPUT);
  pinMode(LEDPin, OUTPUT);
  delay(100);
 
  
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("\nValor = ");
  Serial.println(c++);// print and increment the data

  
  // Leer el valor del potenciómetro (0-1023)
  int potValue = analogRead(pot);
  int LEDSpeed = map(potValue, 0, 1023, 0, 255);
  analogWrite(LEDPin, LEDSpeed);
  Serial.print("Potencia:");
  Serial.println(potValue);
  //Serial.print("Brillo del Led:");
  //Serial.println(LEDSpeed);
  // Mapear el valor del potenciómetro a la velocidad del ventilador (0-255)
  

  // Establecer la velocidad del LED
  

  delay(100);
  

  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > watchdog){
    previousMillis = currentMillis;
    WiFiClient client;// creating the client

    // checking if we can connect to the server
    if(!client.connect(host, port)){
      Serial.println("Fail to connect");
      return;
    }

    // building our url according our database
    String url = "/ESP8266/index.php?cont=";
    url += c;
    url += "&pot=";
    url += potValue;
    //Print data to the server that a client is connected to
    client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

    // if watchdog timeout, the client is stopped
    unsigned long timeout = millis();
    while(client.available() == 0){
      if(millis() - timeout > 5000){
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }

    //Read reply from the server
    while(client.available()){
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
  }
  delay(2000);
}
