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
const char* mqtt_led_control_topic = "weather/station/led/control";
const char* mqtt_led_status_topic = "weather/station/led/status";

// ================= DHT ==================
#define DHTPIN 5         // GPIO5
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// ================= LED ==================
#define LED_PIN 4 // GPIO4 → D2

// ================= GLOBAL VARIABLES ===============
float temperature = 0;
float humidity = 0;
bool ledState = false;  // Track LED state

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

// 💡 MQTT CALLBACK FUNCTION - Handle incoming messages
void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  Serial.print(F("Message arrived ["));
  Serial.print(topic);
  Serial.print(F("] "));
  Serial.println(message);
  
  // Handle LED control
  if (String(topic) == mqtt_led_control_topic) {
    message.trim();
    message.toUpperCase();
    if (message == "ON") {
      digitalWrite(LED_PIN, HIGH);
      ledState = true;
      client.publish(mqtt_led_status_topic, "ON");
      Serial.println(F("LED turned ON"));
    } else if (message == "OFF") {
      digitalWrite(LED_PIN, LOW);
      ledState = false;
      client.publish(mqtt_led_status_topic, "OFF");
      Serial.println(F("LED turned OFF"));
    } else {
      Serial.print(F("Invalid LED command: "));
      Serial.println(message);
    }
  }
}

void reconnect_mqtt() {
  while (!client.connected()) {
    Serial.print(F("Connecting to MQTT..."));

    if (client.connect("ESP8266_WeatherStation")) {
      Serial.println(F("connected"));
      // Subscribe to LED control topic
      client.subscribe(mqtt_led_control_topic);
      Serial.print(F("Subscribed to: "));
      Serial.println(mqtt_led_control_topic);
      // Publish initial LED status
      client.publish(mqtt_led_status_topic, ledState ? "ON" : "OFF");
      // Publish initial sensor data immediately after connection
      Serial.println(F("Publishing initial sensor data..."));
      publish_sensor_data();
    } else {
      Serial.print(F("failed, rc="));
      Serial.print(client.state());
      Serial.println(F(" retrying in 5 seconds"));
      delay(5000);
      // Blink LED while reconnecting
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
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
  read_sensor();

  // Create JSON payload with numeric values (not quoted numbers)
  String payload = "{";
  payload += "\"temperature\":" + String(temperature, 2) + ",";
  payload += "\"humidity\":" + String(humidity, 2);
  payload += "}";
  
  // Note: String(temperature, 2) creates a numeric string without quotes
  // This should produce: {"temperature":37.30,"humidity":64.00}

  Serial.print(F("Temperature: "));
  Serial.print(temperature, 2);
  Serial.print(F(" °C, Humidity: "));
  Serial.print(humidity, 2);
  Serial.println(F(" %"));

  // Publish to MQTT
  if (client.publish(mqtt_topic, payload.c_str())) {
    Serial.print(F("✓ Published to "));
    Serial.print(mqtt_topic);
    Serial.print(F(": "));
    Serial.println(payload);
  } else {
    Serial.println(F("✗ Failed to publish data!"));
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
  client.setCallback(callback);  // Set callback function
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
