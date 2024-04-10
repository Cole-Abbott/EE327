#include <Arduino.h>

#define LED_BUILTIN 2


void setup() {
  // put your setup code here, to run once:
  Serial.begin(112500);
  Serial.println("Hello World");

  //blink led
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);


}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Hello World");
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
  digitalWrite(LED_BUILTIN, HIGH);
}

