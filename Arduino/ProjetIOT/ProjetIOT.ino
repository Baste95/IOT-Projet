// Include
#include <Wire.h>
#include <SPI.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// Define
#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10
#define FIREBASE_HOST "https://iot-capteur-meteo-pollution.firebaseio.com/"
#define FIREBASE_AUTH "EW86IRpKvCfALwMlkDRUQj1avhLuOWWq8ZPf2ZuA"
#define WIFI_SSID "SFR_D838"
#define WIFI_PASSWORD "FamillePelmard"
#define SEALEVELPRESSURE_HPA (1013.25)

// Variables
Adafruit_BME280 bme; // I2C
FirebaseData firebaseData;
FirebaseJson json;
FirebaseJson json2;

unsigned long delayTime;

void setup() {
    Serial.begin(115200);
    while(!Serial);    // time to get serial running
    Serial.println(F("BME280 test"));

    unsigned status;
    
    // default settings
    status = bme.begin(0x76);  
    // You can also pass in a Wire library object like &Wire2
    // status = bme.begin(0x76, &Wire2)
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        while (1) delay(10);
    }
    
    Serial.println("-- Default Test --");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
      Serial.print(".");
      delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();
   
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Firebase.reconnectWiFi(true);
   
    //Set database read timeout to 1 minute (max 15 minutes)
    Firebase.setReadTimeout(firebaseData, 1000 * 60);
    //tiny, small, medium, large and unlimited.
    //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
    Firebase.setwriteSizeLimit(firebaseData, "tiny");
    delayTime = 900000;

    Serial.println();
}


void loop() { 
    float temp = bme.readTemperature();
    float hum = bme.readHumidity();
    float pres = bme.readPressure() / 100.0F;
    json.set("temperatue", temp);
    json.set("humidity", hum);
    json.set("pressure", pres);
    //json2.set(cstr, json);
    Firebase.pushJSON(firebaseData,"/data",json);
    Firebase.pushTimestamp(firebaseData, "/time");
    
    delay(delayTime);
}


void printValues() {
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");

    Serial.print("Pressure = ");

    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    Serial.println();
}
