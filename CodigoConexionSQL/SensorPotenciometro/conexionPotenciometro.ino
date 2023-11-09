#include <ESP8266WiFi.h>
// datos para la conexion al sensor de temperatura y humedad
const int pot = A0;
const int r = D2;
const int g = D3;
const int b = D4;

const char* ssid = "Sofi-IoT";
const char* password = "password";
const char* host = "172.20.10.11";// IP server, obtained with 'ipconfig' in command shell
const int port = 80;

const int watchdog = 5000;// watchdog frequency, reconnect timeout
unsigned long previousMillis = millis();
int c = 0;// the data

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);// initialize the serial port
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
  pinMode(r,OUTPUT);
  pinMode(g,OUTPUT);
  pinMode(b,OUTPUT);
  delay(100);
}

String getColor(int pRead){
  if(pRead >= 6 && pRead <= 176){
    digitalWrite(r,1);
    digitalWrite(g,0);
    digitalWrite(b,0);
    return "Rojo";
  } else if (pRead >= 177 && pRead <= 347){
    digitalWrite(r,1);
    digitalWrite(g,1);
    digitalWrite(b,0);
    return "Amarillo";
  } else if (pRead >= 348 && pRead <= 518){
    digitalWrite(r,0);
    digitalWrite(g,1);
    digitalWrite(b,0);
    return "Verde";
  } else if (pRead >= 519 && pRead <= 689){
    digitalWrite(r,0);
    digitalWrite(g,1);
    digitalWrite(b,1);
    return "Cyan";
  } else if (pRead >= 690 && pRead <= 860){
    digitalWrite(r,0);
    digitalWrite(g,0);
    digitalWrite(b,1);
    return "Azul";
  } else {
    digitalWrite(r,1);
    digitalWrite(g,0);
    digitalWrite(b,1);
    return "Morado";
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("\nValor = ");
  Serial.println(c++);// print and increment the data
  
  int potRead = analogRead(pot);
  Serial.print("potenciometro:");
  Serial.println(potRead);
  String color = getColor(potRead);
  

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
    String url = "/test/index.php?cont=";
    url += c;
    url += "&pot=";
    url += potRead;
    url += "&color=";
    url += color;

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