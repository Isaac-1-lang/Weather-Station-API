#include <ESP8266WiFi.h>

// WiFi credentials
const char* ssid = "precieux";
const char* password = "121402pr0732021";

void setup() {
  Serial.begin(115200);
  delay(1000);

  setup_wifi();
}

void setup_wifi() {
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Connected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Nothing here yet
}
