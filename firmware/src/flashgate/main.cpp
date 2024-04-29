#include "Config.h"
#include "WifiUtils.h"
#include "HttpServer.h"
#include "FileSystem.h"
#include "movingAvg.h"


#define LIGHT_SENSOR_PIN 34 // ESP32 pin GIOP36 (ADC0)
#define EXTERNAL_LED 21
#define INTERNAL_LED 2 
#define N 15
#define SHORT_PULSE_WINDOW 6
#define LONG_PULSE_WINDOW 9
#define THRESHOLD 300.0

uint8_t byte_index = 0;
uint16_t history[N];

void create_samples(uint16_t sample)
{
  uint16_t previous = history[byte_index] ;  // get oldest from buffer
  history[byte_index] = sample ;  // insert newest
  byte_index++ ;    // move pointer circularly
  // use milis function instead of delay(way )
    if (byte_index >= N){
    byte_index = 0 ;
  }
}


// Function to compare averages of two sets of N elements each
int detectPulses(int size) {
    // Calculate the average of the first N elements
    double avg1 = 0;
    for (int i = 0; i < N; i++) {
        avg1 += history[i];
    }
    avg1 /= (double)N;

    // Calculate the average of the next 10 elements
    double avg2 = 0;
    for (int i = N-size; i < N; i++) {
        avg2 += history[i];
    }
    avg2 /= double(size);
    // Serial.printf("Avg1:%.3f\n", avg1);
    // Serial.printf("Avg2:%.3f\n", avg2);
    
    double diff = avg1 - avg2;
    // Serial.printf("Diff:%.3f\n", diff);

    // Compare averages
    if (diff >=THRESHOLD)
        return 1;
    else
        return 0;
}




void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  setupFileSystem();
  setupWifi();
  setupHttpServer();
  pinMode(EXTERNAL_LED,OUTPUT);
  pinMode(INTERNAL_LED,OUTPUT);
  pinMode(LIGHT_SENSOR_PIN,INPUT);
  digitalWrite(EXTERNAL_LED, LOW);
  digitalWrite(INTERNAL_LED, LOW);

}

void loop() {
  // reads the input on analog pin (value between 0 and 4095)
  int sensorData = analogRead(LIGHT_SENSOR_PIN);
  uint16_t sample = (uint16_t) sensorData;
  unsigned long startTime = millis();
  bool longPulse = false;

  create_samples (sample) ;

  Serial.printf("Raw:%d\n", sensorData);
  // if (detectPulses(LONG_PULSE_WINDOW)==1)
  // {
  //   longPulse = true;
  //   Serial.printf("Long Pulse Detected\n");
  //   digitalWrite(INTERNAL_LED, HIGH); // turn on LED
  //   while (millis() - startTime < 100) {
  //   }
  // }
  // else 
  if (detectPulses(SHORT_PULSE_WINDOW)==1){
    Serial.printf("Short Pulse Detected\n");
    digitalWrite(EXTERNAL_LED, HIGH); // turn on LED
    while (millis() - startTime < 100) {
    }
  }
  else {
    digitalWrite(EXTERNAL_LED, LOW);  // turn off LED
    digitalWrite(INTERNAL_LED, LOW); 
  }
  
  
  while (millis() - startTime < 50) {
    // Do nothing, just wait
  }
}
