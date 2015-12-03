nclude <DHT22.h>

// Barometer
#include <Barometer.h>
#include <Wire.h>

// REST-Client
#include <SPI.h>
#include <Ethernet.h>
#include "RestClient.h"

// PIN Definitionen
#define BUTTON_PIN 2
#define DHT_PIN A3

// Konstanten
const int DURATION_MEASUREMENTS_MILLIS = 4000;
const int DURATION_NEW_MEASURING_INTERVAL_MILLIS = 3000;

// REST-Server
RestClient restClient = RestClient("192.168.17.115",80);

// MAC & IP Address of arduino
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192, 168, 17, 10 };

// Sensoren
DHT22 dht = DHT22(DHT_PIN);
Barometer myBarometer;

void setup()
{
  // Serial
  Serial.begin(9600);
  Serial.println("start");
  
  //Ethernet
  Ethernet.begin(mac,ip);
    
  // Button
  pinMode(BUTTON_PIN, INPUT);
  
  // Barometer
  myBarometer.init();
}

void loop()
{
  delay(150);
  
  int intervalLoop = 500;
    
  int currentIntervalMessungen = 0;
  int currentIntervalButton = 0;
  
  while(true)
  {
    // Sensoren-Messungen
    if (currentIntervalMessungen >= DURATION_MEASUREMENTS_MILLIS) {
      currentIntervalMessungen = 0;
      
      // Read sensor values
      float airPressure = myBarometer.bmp085GetPressure(myBarometer.bmp085ReadUP());
      dht.readData();
      int humidity = dht.getHumidityInt() / 10;
      int temperature = dht.getTemperatureCInt() / 10;
      
      // ContenType
      String contentType = "application/json ";
      char contentTypeArray[contentType.length()];
      contentType.toCharArray(contentTypeArray, contentType.length());
      restClient.setContentType(contentTypeArray);
      
      // Create rest json paramaters
      String request = "{\"temperature\": " + String(temperature) + "," + "\"humidity\": " + String(humidity) + "," + "\"pressure\": " + String((int)(airPressure/100)) + "}";
      char requestArray[request.length() + 1];
      request.toCharArray(requestArray, request.length() + 1);
            
      // Send REST Request
      Serial.println(request);
      int statusCode = restClient.post("/measurements.json", requestArray);
    }
    currentIntervalMessungen = currentIntervalMessungen + intervalLoop;
    
    // Neues Mess-Interval
    int buttonState = digitalRead(BUTTON_PIN);
    if (buttonState == HIGH) {
      currentIntervalButton = currentIntervalButton + intervalLoop;
      if (currentIntervalButton >= DURATION_NEW_MEASURING_INTERVAL_MILLIS) {
        Serial.println("start new interval");
        int statusCode = restClient.post("/intervals.json", "");
        currentIntervalButton = 0;
      }      
    } else {
      currentIntervalButton = 0;
    }
    
    delay(intervalLoop);
  }
}
