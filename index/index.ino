#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// ================= WIFI =================
const char* ssid = "precieux";
const char* password = "121402pr0732021";

// ================= MQTT =================
const char* mqtt_server = "157.173.101.159";
const int mqtt_port = 1883;
const char* mqtt_topic = "weather/station/data";

// ================= DHT ==================
#define DHTPIN 5         // GPIO5
#define DHTTYPE DHT11     // or DHT22

DHT dht(DHTPIN, DHTTYPE);

// ================= GLOBAL VARIABLES ===============
float temperature = 0;   // Persist in RAM
float humidity = 0;

WiFiClient espClient;
PubSubClient client(espClient);

// ================= FUNCTIONS ==============
void setup_wifi() {
  Serial.print(F("Connecting to WiFi"));

  WiFi.mode(WIFI_STA);     
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println(F("\nWiFi connected"));
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void reconnect_mqtt() {
  while (!client.connected()) {
    Serial.print(F("Connecting to MQTT..."));
    
    if (client.connect("ESP8266_WeatherStation")) {
      Serial.println(F("connected"));
    } else {
      Serial.print(F("failed, rc="));
      Serial.print(client.state());
      Serial.println(F(" retrying in 5 seconds"));
      delay(5000);
    }
  }
}

// 🌡️ SENSOR READ FUNCTION
void read_sensor() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    temperature = 0;
    humidity = 0;
  }
}

// 📡 PUBLISH SENSOR DATA FUNCTION
void publish_sensor_data() {
  read_sensor(); // updates global temperature & humidity

  // JSON payload for MQTT
  String payload = "{";
  payload += "\"temperature\":" + String(temperature, 2) + ",";
  payload += "\"humidity\":" + String(humidity, 2);
  payload += "}";

  // 📢 Human-readable serial print
  Serial.print(F("Temperature: "));
  Serial.print(temperature, 2);
  Serial.print(F(" °C, Humidity: "));
  Serial.print(humidity, 2);
  Serial.println(F(" %"));

  client.publish(mqtt_topic, payload.c_str());
}

// ================= SETUP ==================
void setup() {
  Serial.begin(115200);
  delay(1000);

  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

// ================= LOOP ===================
void loop() {
  if (!client.connected()) {
    reconnect_mqtt();
  }

  client.loop();
  publish_sensor_data();

  delay(5000); // send every 5 seconds
}

// void setup() {
//   Serial.begin(115200);
//   delay(2000);
//   Serial.println("Hello, ESP8266!");
// }

// void loop() {
//   Serial.println("Running...");
//   delay(1000);
// }

