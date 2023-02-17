/*
Author: Erientes
GH: github.com/Erientes

This is a very hacky way of making a blinking LED on the ESP32-WROOM.
This board has no builtin LED, but it does have a TX LED.
So the way this script work is that it sends a bunch of garbage data to the TX port.
As long as TX is active, the TX LED will light up.

*/
int time_led_on = 1000;
int time_led_off = 1000;

void led_on(int t_target){
  int i = 0;
  while(millis() < t_target){
    Serial.print(".");
    i++;
  }
  Serial.println("\n" + (String) i);
}

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
}

// the loop function runs over and over again forever
void loop() {
  led_on(millis() + time_led_on);
  delay(time_led_off);
}
