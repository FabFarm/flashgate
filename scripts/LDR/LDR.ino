/*
Author: Erientes

Simply outputs analog readout from pin 34 (which is A6) to serial.
In the Arduino IDE you can see the live plot by opening the Serial Plotter.

*/

#define PIN_LDR 34

int val = 0;

void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.println(analogRead(PIN_LDR));
  delay(10);
}
