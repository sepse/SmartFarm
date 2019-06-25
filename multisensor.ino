#include <DHT.h>;
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Ticker.h>

#define DHTPIN 23     
#define DHTTYPE DHT22   
DHT dht(DHTPIN, DHTTYPE); 
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

Ticker tickeri;

int i;
int dritaPin = 35;  

//Variables
int chk;
float hum;  //Humidity
float temp; //Temperature

sensors_event_t event;
float drita_perqindje;

int soil = 34; // soil moisture #1
int soil2 = 32; // soil moisture #2

//FlowVariables
byte sensorInterrupt = 33;  // 0 = digital pin 2
byte sensorPin = 33;
float calibrationFactor = 4.5;

volatile byte pulseCount;  

float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

unsigned long oldTime;

// Wifi: SSID and password
const PROGMEM char* WIFI_SSID = "SSID";
const PROGMEM char* WIFI_PASSWORD = "PASSWORD";

// MQTT: ID, server IP, port, username and password
const PROGMEM char* MQTT_CLIENT_ID = "office_dht22";
const PROGMEM char* MQTT_SERVER_IP = "192.168.0.47";
const PROGMEM uint16_t MQTT_SERVER_PORT = 1883;
const PROGMEM char* MQTT_USER = "USER";
const PROGMEM char* MQTT_PASSWORD = "PASSWORD";

const PROGMEM char* MQTT_TEMP_TOPIC = "oborri/temperatura";
const PROGMEM char* MQTT_HUM_TOPIC = "oborri/lageshtia";
const PROGMEM char* MQTT_SOIL_TOPIC = "oborri/dheu";
const PROGMEM char* MQTT_SOIL_TOPIC2 = "oborri/dheu2";
const PROGMEM char* MQTT_PRESS_TOPIC = "oborri/shtypja";
const PROGMEM char* MQTT_DRITA_TOPIC = "oborri/drita";
const PROGMEM char* MQTT_UJI_TOPIC = "oborri/uji";


WiFiClient wifiClient;
PubSubClient client(wifiClient);

// function called when a MQTT message arrived
void callback(char* p_topic, byte* p_payload, unsigned int p_length) {
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("INFO: Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("INFO: connected");
    } else {
      Serial.print("ERROR: failed, rc=");
      Serial.print(client.state());
      Serial.println("DEBUG: try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void all_sensors(){
client.publish(MQTT_TEMP_TOPIC, String(temp).c_str(), true);
client.publish(MQTT_HUM_TOPIC, String(hum).c_str(), true);
client.publish(MQTT_SOIL_TOPIC, String(analogRead(soil)).c_str(), true);
client.publish(MQTT_SOIL_TOPIC2, String(analogRead(soil2)).c_str(), true);
client.publish(MQTT_PRESS_TOPIC, String(event.pressure).c_str(), true);
client.publish(MQTT_DRITA_TOPIC, String(drita_perqindje).c_str(), true);
client.publish(MQTT_UJI_TOPIC, String(totalMilliLitres).c_str(), true);
totalMilliLitres = 0;
}


void setup(){
Serial.begin(115200);
  // init the WiFi connection
  Serial.println();
  Serial.println();
  Serial.print("INFO: Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("INFO: WiFi connected");
  Serial.println("INFO: IP address: ");
  Serial.println(WiFi.localIP());

  // init the MQTT connection
  client.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);
  client.setCallback(callback);
if(!bmp.begin())
  {
    /* There was a problem detecting the BMP085 ... check your connections */
    Serial.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
Serial.println("Start");
Serial.println("");

  pinMode(sensorPin, INPUT);
  //digitalWrite(sensorPin, HIGH);
 pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0;

  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  tickeri.attach(300, all_sensors);

dht.begin();

}

void loop(){
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if((millis() - oldTime) > 1000)    // Only process counters once per second
  { 
    detachInterrupt(sensorInterrupt);
        
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
    
    oldTime = millis();
    
    flowMilliLitres = (flowRate / 60) * 1000;
    
    totalMilliLitres += flowMilliLitres;
      
    unsigned int frac;
    
    Serial.print("Flow rate: ");
    Serial.print(int(flowRate));  
    Serial.print("L/min");
    Serial.print("\t");     
    
    pulseCount = 0;
    
    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  }  
hum = dht.readHumidity();
temp= dht.readTemperature();

Serial.println("Lageshtia Ajrit: " + String(hum));
Serial.println("Temperatura: " + String(temp) + " Celsius");
Serial.print("Lageshtia Dheut: "); Serial.println(analogRead(soil));
Serial.print("Lageshtia Dheut2: "); Serial.println(analogRead(soil2));
int drita = analogRead(dritaPin); 
drita_perqindje = drita / 1023.0;
drita_perqindje = pow(drita_perqindje, 2.0);

Serial.print("Drita: ");
Serial.println(drita_perqindje);

bmp.getEvent(&event);
 if (event.pressure)
  {
    Serial.print("Shtypja: ");
    Serial.print(event.pressure);
    Serial.println(" hPa");
       
  }
  else
  {
    Serial.println("Sensor error");
  }

Serial.println();

delay(1000);

}

void pulseCounter()
{
    pulseCount++;
}
