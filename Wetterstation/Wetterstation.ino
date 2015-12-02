// Temperatur & Luftfeuchtigkeit
#include <DHT22.h>

// Barometer
#include <Barometer.h>
#include <Wire.h>

// REST
#include <SPI.h>
#include <Ethernet.h>
#include "RestClient.h"

// PIN Definitionen
#define BUTTON_PIN 2
#define DHT_PIN A3

// Konstanten
const int DURATION_MEASUREMENTS_MILLIS = 4000;
const int DURATION_NEW_MEASURING_INTERVAL_MILLIS = 3000;

//Rest Init
//IP of target
RestClient restClient = RestClient("192.168.17.115",80);
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
//the IP address for the shield:
byte ip[] = { 192, 168, 17, 10 };

DHT22 dht = DHT22(DHT_PIN);
Barometer myBarometer;

void setup()
{
  // Serial
  Serial.begin(9600);
  
  //Ethernet
  Ethernet.begin(mac,ip);
  Serial.println("set up ");
  
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
      float airPressure;
      airPressure = myBarometer.bmp085GetPressure(myBarometer.bmp085ReadUP());
             
      Serial.print("Pressure: ");
      Serial.print(airPressure, 0); //whole number only.
      Serial.println(" Pa");      
      
      dht.readData();
      int humidity = dht.getHumidityInt() / 10;
      int temperature = dht.getTemperatureCInt() / 10;
      
      //Set Contenttype of the RestClient
      String contentType = "application/json ";
      char contentT[contentType.length()];
      contentType.toCharArray(contentT, contentType.length());
      restClient.setContentType(contentT);
      
      Serial.println(airPressure);
      
      String airPressureString = String((int)(airPressure/100));
      
      // Create rest json paramaters
      String req = "{\"temperature\": " + String(temperature) + "," + "\"humidity\": " + String(humidity) + "," + "\"pressure\": " + airPressureString + "} ";
      char request[req.length()];
      req.toCharArray(request, req.length());
      
      // Send REST Request
      int statusCode = restClient.post("/measurements.json", request);
    }
    currentIntervalMessungen = currentIntervalMessungen + intervalLoop;
    
    // Neues Mess-Interval
    int buttonState = digitalRead(BUTTON_PIN);
    if (buttonState == HIGH) {
      currentIntervalButton = currentIntervalButton + intervalLoop;
      if (currentIntervalButton >= DURATION_NEW_MEASURING_INTERVAL_MILLIS) {
        Serial.println("Start new interval");
        // TODO Send REST Request -> Start new interval
        currentIntervalButton = 0;
      }      
    } else {
      currentIntervalButton = 0;
    }
    
    delay(intervalLoop);
  }
}

