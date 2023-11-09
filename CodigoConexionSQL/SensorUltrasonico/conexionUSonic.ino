#include <ESP8266WiFi.h>

// cosas para el sensor ultrasonico
#define trigPin D8  // Trig pin (GPIO 15)
#define echoPin D7  // Echo pin (GPIO 13)
long duration;
int distance;

// cosas para la conexion al internet
const char* ssid = "Sofi-IoT";
const char* password = "password";
const char* host = "172.20.10.11";// IP server, obtained with 'ipconfig' in command shell
const int port = 80;
const int watchdog = 5000;
unsigned long previousMillis = millis();
int c = 0;


void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  WiFi.begin(ssid, password);
  
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

long getDistance(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration= pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2; // Calculate distance in centimeters
  return distance;
}

void loop() {
  Serial.print("\nValor = ");
  Serial.println(c++);
  
  // Calculating the distance
  distance= getDistance();
  Serial.print("Distance: ");
  Serial.println(distance);
  
  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > watchdog) {
    previousMillis = currentMillis;
    WiFiClient client;

    if (!client.connect(host, port)) {
      Serial.println("Fail to connect");
      return;
    }

    String url = "/test/index.php?cont=";
    url += c;
    url += "&dist=";
    url += distance;

    client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }

    while (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
  }
  
  delay(2000);
}
