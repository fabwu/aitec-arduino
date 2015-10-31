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
#define BAROMETER_PIN A0
#define BUTTON_PIN 2
#define DHT_PIN A3

// Konstanten
const int DURATION_MEASUREMENTS_MILLIS = 4000;
const int DURATION_NEW_MEASURING_INTERVAL_MILLIS = 3000;

RestClient restClient = RestClient("192.168.0.23", 80);
DHT22 dht = DHT22(DHT_PIN);
Barometer myBarometer;

void setup()
{
  // Serial
  Serial.begin(9600);
    
  // Button
  pinMode(BUTTON_PIN, INPUT);
  
  // Barometer
  // myBarometer.init();
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
      int airPressure = analogRead(BAROMETER_PIN);
      //airPressure = barometer.bmp085GetPressure(barometer.bmp085ReadUP());
      
      dht.readData();
      int humidity = dht.getHumidityInt();
      int temperature = dht.getTemperatureCInt();
      
      // Create rest paramaters
      String req = "airPressure=" + String(airPressure) + ";humidity=" + String(humidity) + ";temperature=" + String(temperature);
      char request[req.length()];
      req.toCharArray(request, req.length());
      
      // TODO Send REST Request
      Serial.println(req);
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

