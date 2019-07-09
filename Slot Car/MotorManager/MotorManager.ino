#include <Wire.h>
  #include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *myMotor = AFMS.getMotor(1);

const int ledPin = LED_BUILTIN;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  AFMS.begin();
  pinMode(ledPin, OUTPUT);
  setSpeed(0);
}

void loop() {
  // put your main code here, to run repeatedly:
  char speed;
  // check if data has been sent from the computer:
  if (Serial.available()) {
    speed = Serial.read();
    setSpeed(speed);
    Serial.println(speed);
  }  
  }
  
  void setSpeed(char c){   //ENDRE TIL ASCII-TEGN FOR Å FÅ FLERE STEG
    int i = 0;
    if (c=='1') i=50;
    if (c=='2') i=75;
    if (c=='3') i=100;
    if (c=='4') i=125;
    if (c=='5') i=150;
    if (c=='6') i=175;
    if (c=='7') i=200;
    if (c=='8') i=225;
    if (c=='9') i=255;
    myMotor->setSpeed(i);
    myMotor->run(FORWARD);
    Serial.println(i);
    
  /*
  Serial.println ("Run");
  myMotor->setSpeed(250);
  myMotor->run(FORWARD);
  delay (3000);
  Serial.println ("stop");
  myMotor->setSpeed(0);
  myMotor->run(FORWARD);
  delay (3000);
  */
  

}
