/*
Author: Erientes

GPIO pin 34 == A6
*/

#define PIN_LDR 34
#define PIN_LED 32

int val = 0;

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
}

void loop() {
//  digitalWrite(PIN_LED, HIGH);
  Serial.println(analogRead(PIN_LDR));
  delay(10);
}
