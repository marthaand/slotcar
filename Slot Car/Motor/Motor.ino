#include <Wire.h>
  #include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *myMotor = AFMS.getMotor(1);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  AFMS.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println ("Run");
  myMotor->setSpeed(255);
  myMotor->run(FORWARD);
  delay (3000);
  Serial.println ("stop");
  myMotor->setSpeed(0);
  myMotor->run(FORWARD);
  delay (3000);
  

}
