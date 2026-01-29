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
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// ================= LED ==================
#define LED_PIN 4 // GPIO4 → D2

// ================= GLOBAL VARIABLES ===============
float temperature = 0;
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
    // Blink LED while connecting
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  }

  digitalWrite(LED_PIN, LOW); // Turn LED off when connected
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
      // Blink LED while reconnecting
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }
  }
  digitalWrite(LED_PIN, HIGH); // LED ON when MQTT connected
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
  read_sensor();

  String payload = "{";
  payload += "\"temperature\":" + String(temperature, 2) + ",";
  payload += "\"humidity\":" + String(humidity, 2);
  payload += "}";

  Serial.print(F("Temperature: "));
  Serial.print(temperature, 2);
  Serial.print(F(" °C, Humidity: "));
  Serial.print(humidity, 2);
  Serial.println(F(" %"));

  client.publish(mqtt_topic, payload.c_str());

  // Example LED alert: turn LED on if temperature > 30°C
  if (temperature > 30) {
    digitalWrite(LED_PIN, HIGH);
  } else if (client.connected()) {
    digitalWrite(LED_PIN, HIGH); // LED ON when MQTT OK
  } else {
    digitalWrite(LED_PIN, LOW);  // LED OFF otherwise
  }
}

// ================= SETUP ==================
void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

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
  delay(5000);
}
