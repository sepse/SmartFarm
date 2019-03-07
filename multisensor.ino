#include <DHT.h>;
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <SPI.h>
#include <Wire.h>

#define DHTPIN 23     
#define DHTTYPE DHT22   
DHT dht(DHTPIN, DHTTYPE); 
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

int i;
int drita = 0;  
//Variables
int chk;
float hum;  //Humidity
float temp; //Temperature

int soil = 4; // soil moisture

void setup(){
Serial.begin(9600);
if(!bmp.begin())
  {
    /* There was a problem detecting the BMP085 ... check your connections */
    Serial.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
Serial.println("Start");
Serial.println("");

dht.begin();

}

void loop(){

hum = dht.readHumidity();
temp= dht.readTemperature();
Serial.println("Lageshtia Ajrit: " + String(hum));
Serial.println("Temperatura: " + String(temp) + " Celsius");
Serial.print("Lageshtia Dheut: "); Serial.println(analogRead(soil));
drita = analogRead(0); 
Serial.print("Drita: ");
Serial.println(drita);
sensors_event_t event;
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

delay(3000);

}
