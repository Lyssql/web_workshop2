#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <DHT.h>

// Define sensor pins and type
#define TRIG_PIN 5  // D1 -> GPIO5
#define ECHO_PIN 4  // D2 -> GPIO4
#define DHT_PIN 2   // D4 -> GPIO2 
#define DHT_TYPE DHT11  

DHT dht(DHT_PIN, DHT_TYPE);  // Initialize DHT sensor

WebSocketsServer webSocket = WebSocketsServer(81);  // WebSocket on port 81

const char* ssid = "PoleDeVinci_IFT";
const char* password = "c.r4UV@VfPn_0";

// Ultrasonic sensor
long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  long distance = duration * 0.034 / 2;  // Convert to cm
  return distance;
}

// WebSocket event handler
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {
    if (strcmp((char*)payload, "get_data") == 0) {
      float temp = dht.readTemperature();
      float humidity = dht.readHumidity();
      long distance = getDistance();
      String json = "{\"temp\":" + String(temp) + ",\"humidity\":" + String(humidity) + ",\"distance\":" + String(distance) + "}";
      webSocket.sendTXT(num, json);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // WiFi Setup
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  // Initialize sensors
  dht.begin();

  // Initialize ultrasonic sensor
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // WebSocket Setup
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();  // Handle WebSocket communication
}

