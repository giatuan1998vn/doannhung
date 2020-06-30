//--------------------------------------------------------------------------
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#include <ArduinoJson.h>
#define MQTT_VERSION MQTT_VERSION_3_1_1

// Wifi: 
const char* WIFI_SSID = "HTTP";
const char* WIFI_PASSWORD = "1234567890";

// MQTT: 
const PROGMEM char* MQTT_CLIENT_ID = "tuan";  
const PROGMEM char* MQTT_SERVER_IP = "192.168.43.62";
const PROGMEM uint16_t MQTT_SERVER_PORT = 1883;
const PROGMEM char* MQTT_USER = "tuan";
const PROGMEM char* MQTT_PASSWORD = "tuan";


//-----------------DHT11-----------------------------
#define DHTPIN 2
#define DHTTYPE DHT11

#define LED2 16
#define LED3 0
 
long lastMsg = 0;
char msg[50];
int value = 0;
// Timer giá trị
long now = millis();
long lastMeasure = 0;
DHT dht(DHTPIN, DHTTYPE);
float h,t;
WiFiClient wifiClient;
PubSubClient client(wifiClient);


//---------------------------------------------------------------------------
// function called when a MQTT message arrived
void callback(char* p_topic, byte* p_payload, unsigned int p_length) {
  // concat the payload into a string
  String payload;
  for (uint8_t i = 0; i < p_length; i++) {
    payload.concat((char)p_payload[i]);
  }
}
//---------------------------------------------------------------------
void reconnect() {
  // ------kết nối mqtt -------------------
  while (!client.connected()) {
    Serial.println("INFO: Attempting MQTT connection...");
    // Connect MQTT
    if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("INFO: connected");
      digitalWrite(LED2,HIGH);
      } 
    else {
      Serial.print("ERROR: ");
      Serial.print(client.state());
      Serial.println("  seconds");
      // Wait 5 seconds 
      delay(5000);
    }
  }
}
//=========================================================================
void setup() {
  // init the serial
  Serial.begin(115200);
  //-------------------------------
   dht.begin();
   pinMode(LED2,OUTPUT);
   pinMode(LED3,OUTPUT);
   
  //---------------------------------------

  // init the WiFi connection
  Serial.println();
  Serial.println();
  Serial.print("INFO: Connecting to ");
  WiFi.mode(WIFI_STA);
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("INFO: WiFi connected");
  Serial.print("INFO: IP address: ");
  Serial.println(WiFi.localIP());

  // init the MQTT connection
  client.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

//--------------------------------------------------------
  now = millis();
  // Publishes new temperature and humidity every 30 seconds
  if (now - lastMeasure > 1000) {
    lastMeasure = now;
     h = dht.readHumidity();
     t = dht.readTemperature();
    if (isnan(h) /*|| isnan(l)*/) {
      Serial.println("Failed to read from DHT sensor!");
      digitalWrite(LED3,LOW);
      delay(10);
      return;
    }
   
//    static char humidityTemp[7];
//    dtostrf(h, 6, 2, humidityTemp);
//    // Publishes Temperature and Humidity values
//    client.publish("sensor/humidity", humidityTemp);
//    
//    Serial.print("Humidity: ");
//    Serial.print(h);
//    Serial.print("\n ");
//    
    static char temperatureTemp[7];
    dtostrf(t, 6, 2, temperatureTemp);
    
    static char humidityTemp[7];
    dtostrf(h, 6, 2, humidityTemp);

    // Publishes Temperature and Humidity values
    client.publish("sensor/temperature", temperatureTemp);
    client.publish("sensor/humidity", humidityTemp);
    
    Serial.print("Độ ấm: ");
    Serial.print(h);
    Serial.print(" %\t Nhiệt độ: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print("\n ");
  }
  if (h=0){digitalWrite(LED3, HIGH);
    delay(500);}
  
//--------------------------------------------------------
}
