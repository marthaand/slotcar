//--------------------------------------------------------------------------------
//[SC:SENSOR:1:1]
// SC030003 - Standalone on Arduino UNO
// SC030004 - HW test : Blink on Arduino, output on 8 and 12
// SC030005 - Frontend Processor - output on 8 and 12
// SC030006 - DisplayProcessor input 14,16 -> 22,23
// SC030007 - 2 X 15 seg. LED and NEO-LED display
// Sc030008 - Slave sensors listening on serial1
// SC030020 - Testing slave boards : [S:AC]
//--------------------------------------------------------------------------------
//I2C device found at address 0x40 - 64
//I2C device found at address 0x70 - 112
//I2C device found at address 0x72 - 114
//--------------------------------------------------------------------------------
#include <Adafruit_NeoPixel.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

char *message = "Hello world!     ";
char *driverInfo="????";
char displaybuffer[4] = {'1', '2', '3', '4'};
int raceSecPrev=0;


Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();
Adafruit_AlphaNum4 alpha1 = Adafruit_AlphaNum4();


#ifdef __AVR__
#include <avr/power.h>
#endif
// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            6

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      32

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int delayval = 500; // delay for half a second
//--------------------------------------------------------
// constants won't change. Used here to set pin numbers:
  // Pin 13: Arduino has an LED connected on pin 13
  // Pin 11: Teensy 2.0 has the LED on pin 11
  // Pin  6: Teensy++ 2.0 has the LED on pin 6
  // Pin 13: Teensy 3.0 has the LED on pin 13
const int ledPin =  13;      // the number of the LED pin

// Variables will change:
int racenum=0;
int ledState = LOW;             // ledState used to set the LED
long previousMillis = millis();        // will store last time LED was updated

// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long interval = 1000;           // interval at which to blink (milliseconds)
  unsigned long currentMillis = millis();
boolean DebugBlink=false;
boolean DebugSerialChar=false;
boolean DS=false;
boolean D=false;
boolean blink=false;
boolean BlinkOutput=false;
boolean sensorsOnly=false;
boolean DisableSerial=false; // not implemented
boolean HWtest=false;
boolean HWtestNEO=true;
boolean LedDisplay=true;

String RaceStatus="PREPARE";  // PREPARE,COUNT,START,RUN,FINISH,UPLOAD
String TrackStatus[]={"PREPARE","PREPARE"};
String SensorStatus[]={"NO","NO"};
String msgUniversalInput[]={"","","",""};
int sensorCnt[]={0,0};
int CountDown=10;
long CountDownPreviousMillis = millis();        // will store last time LED was updated
long RaceStartMillis = millis();        // will store last time LED was updated
long FinishTime[]={0,0};
int LapCount[]={0,0};
int LapTime[2][3];
//int LapCountNew[]={0,0};
long previousDetect0[]={0,0};
long previousDetect1[]={0,0};
int previousSensor[]={0,0};
//byte sensorPin[]={2,4};  // Arduino UNO
//byte sensorPin[]={14,16};  // Arduino feather
byte sensorPin[]={22,23};  // Arduino feather
byte OutputPin[]={8,12};

// constants won't change. They're used here to 
// set pin numbers:
//const int buttonPin = 2;     // the number of the pushbutton pin
//const int ledPin =  13;      // the number of the LED pin

// Variables will change:
//int ledState2 = HIGH;         // the current state of the output pin
int buttonState[2];             // the current reading from the input pin
int lastButtonState[] = {LOW,LOW};   // the previous reading from the input pin

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime[] = {0,0};  // the last time the output pin was toggled
//long debounceDelayOn = 5;    // the debounce time; increase if the output flickers
long debounceDelayOn = 5;    // the debounce time; increase if the output flickers
long debounceDelayOff = 2500;    // the debounce time; increase if the output flickers



void setup() {
  lastButtonState[0]=0;
  lastButtonState[1]=0;
  
  pinMode(22, INPUT_PULLUP);
  pinMode(23, INPUT_PULLUP);
  pinMode(22, INPUT);
  pinMode(23, INPUT);
  if (LedDisplay==false){
    
  
    //if (HWtest) hwtest();
    if (HWtest) hwtest2();
    if (sensorsOnly) debounceDelayOff = 1000; 
  }
    pinMode(ledPin, OUTPUT);
    pinMode(OutputPin[0], OUTPUT);
    pinMode(OutputPin[1], OUTPUT);

  openSerial();
  pixels.begin(); // This initializes the NeoPixel library.
  neoAllOff();
  delay (1000);
  neoAllWeak();
  delay (1000);
  neoAllOff();
  neoRed(1,1,true);
  delay (1000);
  if (HWtestNEO){
    hwtestNEO();
  }
  setupLed();
}
void setupLed(){
  // This is the auto-speed doubler line, keep it in, it will
  // automatically double the speed when 16Mhz is selected!
  //if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  
  alpha4.begin(0x70);  // pass in the address
  alpha1.begin(0x72);  // pass in the address

  alpha1.writeDigitRaw(3, 0x0);
  alpha1.writeDigitRaw(0, 0xFFFF);
  alpha1.writeDisplay();
  delay(200);
  alpha4.writeDigitRaw(3, 0x0);
  alpha4.writeDigitRaw(0, 0xFFFF);
  alpha4.writeDisplay();
  delay(200);
  alpha4.writeDigitRaw(3, 0x0);
  alpha4.writeDigitRaw(0, 0xFFFF);
  alpha4.writeDisplay();
  delay(200);
  alpha4.writeDigitRaw(3, 0x0);
  alpha4.writeDigitRaw(0, 0xFFFF);
  alpha4.writeDisplay();
  delay(200);
  alpha4.writeDigitRaw(0, 0x0);
  alpha4.writeDigitRaw(1, 0xFFFF);
  alpha4.writeDisplay();
  delay(200);
  alpha4.writeDigitRaw(1, 0x0);
  alpha4.writeDigitRaw(2, 0xFFFF);
  alpha4.writeDisplay();
  delay(200);
  alpha4.writeDigitRaw(2, 0x0);
  alpha4.writeDigitRaw(3, 0xFFFF);
  alpha4.writeDisplay();
  delay(200);
  
  alpha4.clear();
  alpha4.writeDisplay();

  // send a message!
  for (uint8_t i=0; i<strlen(message)-4; i++) {
    alpha4.writeDigitAscii(0, message[i]);
    alpha4.writeDigitAscii(1, message[i+1]);
    alpha4.writeDigitAscii(2, message[i+2]);
    alpha4.writeDigitAscii(3, message[i+3]);
    alpha4.writeDisplay();
    
    delay(200);
    Ddrivers();
  }
}

void hwtestNEO(){
  Serial.println ("HW Test NEO");
  delay(1000);
  neoAllYellow();
  delay(1000);
  neoAllOff();
  delay(1000);
  neoAllOff();
  neoYellow(2,2,true);
  delay(5000);
  neoAllOff();
  for (int i = 1; i < 1000; i++) {
    hwmonitor();
    delay(20);
  }
  Serial.println ("HW Test NEO end");
}
void hwtest(){
  Serial.begin(9600);
  Serial.println("HWtest");
  while (true){
    hwmonitor();
    Serial.print (digitalRead(sensorPin[0]));
    Serial.print ("-");
    Serial.println (digitalRead(sensorPin[1]));
    delay (250);
  }
}
void hwmonitor(){
    if (digitalRead(sensorPin[0])==HIGH) neoBlue(2,2,true); else neoWhite(2,2,true);
    if (digitalRead(sensorPin[1])==HIGH) neoBlue(7,2,true); else neoWhite(7,2,true);
}
void hwmonitor2(){
    if (buttonState[0]==1) neoBlue(3,2,true); else neoWhite(3,2,true);
    if (buttonState[1]==1) neoBlue(6,2,true); else neoWhite(6,2,true);
    if (lastButtonState[0]==1) neoBlue(4,2,true); else neoWhite(4,2,true);
    if (lastButtonState[1]==1) neoBlue(5,2,true); else neoWhite(5,2,true);
}
void hwtest2(){
  Serial.begin(9600);
  Serial.println("HWtest2");
  while (true){
    sensorHandler(0);
    sensorHandler(1);
    delay (50);
  }
}
void loop(){

  hwmonitor();
  hwmonitor2();
  long t1=micros();
  sensorHandler(0);
  long t2=micros();
  sensorHandler(1);
  long t3=micros();

  if (blink) refreshLed();
  if (sensorsOnly==false) SCupdateTimers();
  long t4=micros();

  if (sensorsOnly==false) raceHandler(0,0);
  long t5=micros();

   if (sensorsOnly==false) handleSerial();
  long t6=micros();

  boolean printTime=false;
  if (printTime){
    long tp=micros();
    Serial.print("LoopTimer:");
    Serial.print (t2-t1);
    Serial.print (":");
    Serial.print (t3-t2);
    Serial.print (":");
    Serial.print (t4-t3);
    Serial.print (":");
    Serial.print (t5-t4);
    Serial.print (":");
    Serial.print (t6-t5);
    Serial.print ("->");
    Serial.print (t6-t1);
    Serial.print ("[");
    Serial.print (micros()-tp);
    Serial.println ("]");
    delay(5000);
  }
}

void sensorHandler(int trackid){
  // read the state of the switch into a local variable:
  int reading = digitalRead(sensorPin[trackid]);
 // if (reading==HIGH) reading=LOW; else reading=HIGH;   // not used for backend processor

  // check to see if you just pressed the button 
  // (i.e. the input went from LOW to HIGH),  and you've waited 
  // long enough since the last press to ignore any noise:  

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState[trackid]) {
    // reset the debouncing timer
    lastDebounceTime[trackid] = millis();
  } 
  long debounceDelay=debounceDelayOn;
  if (reading==LOW){
    debounceDelay=debounceDelayOff;
  }
  if ((millis() - lastDebounceTime[trackid]) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
    buttonState[trackid] = reading;
    handleSensorChange(trackid,reading);
  }
  
  // set the LED using the state of the button:
  digitalWrite(ledPin, buttonState[trackid]);
  digitalWrite(OutputPin[trackid], buttonState[trackid]);


  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState[trackid] = reading;
}
void handleSensorChange(int sensor, int value){
  if (SensorStatus[sensor]=="NO"){
    if (value==1){
      Serial.print ("[SENSOR:START:TRACK:");
      Serial.print (sensor);
      Serial.print (",State:");
      Serial.print (value);
      Serial.print ("]");
      SensorStatus[sensor]="YES";
      sensorDetect(sensor,value); 
      return;
    }
  }    
  if (SensorStatus[sensor]=="YES"){
    if (value==0){
      Serial.print ("TRACK:");
      Serial.print (sensor);
      Serial.print (",State:");
      Serial.println (value);
      SensorStatus[sensor]="NO"; 
      sensorDetect(sensor,value); 
    }
  }    
}

void openSerial(){
//  Serial.begin(230400);
//  Serial.begin(115200);
  Serial.begin(9600);
  Serial1.begin(9600);
  //delay(5000);
}
void handleSerial(){
  //Serial.println ("HandleSerial");
  //delay(1000);
  char x;
  boolean avail=CharAvailable(0);
  if (avail){
    x=getChar(0);
    handleChar(0,x);
  }
  avail=CharAvailable(1);
  if (avail){
    x=getChar(1);
    handleChar(1,x);
  }
  
}
void handleChar(int sourcePort, char x){
  //Serial.print ("HandleChar:");
  //Serial.println (x);
  HIuniversialInput(sourcePort,x);
}
char getChar(int sp){
  if (sp==0) return Serial.read(); 
  if (sp==1) return Serial1.read(); 
  return ' ';
}
boolean CharAvailable(int sp){
  boolean avail=false;
  if (sp==0){
    if (Serial.available()>0) avail=true;
  }
  if (sp==1){
    if (Serial1.available()>0) avail=true;
  }
  return avail;
}
void LedControl(boolean on){
  if (on) ledState = HIGH; else ledState = LOW;
  digitalWrite(ledPin, ledState);
  if (BlinkOutput){
    digitalWrite(OutputPin[0], ledState);
    digitalWrite(OutputPin[1], !ledState); 
  }
  previousMillis = currentMillis;   
}
void ledTogle(){
  if (DebugBlink) Serial.println ("DebugBlink:");
    if (ledState == LOW)
      LedControl(true);
    else
      LedControl(false);
}
void refreshLed(){
  currentMillis = millis();
  if(currentMillis - previousMillis > interval) {
    
    ledTogle(); 
    previousMillis = currentMillis;   
  }
  
}
void SCupdateTimers(){
  if (RaceStatus=="COUNT"){
   if(millis() - CountDownPreviousMillis > interval){
    CountDownPreviousMillis=millis();
    CountDown--;
    if (CountDown>0) updateDisplay();
    raceHandler(0,0);
   }
  }
  if (RaceStatus=="RUN"){
    updateDisplay();
  }
  
}
void HandleMessage(byte serialPort,String msg){
  Serial.println ("Handle Message2:"+msg);
  Serial.print ("debug serial port:");
  Serial.println (serialPort);
  if (serialPort==1){
    Serial.print ("[RFS:");
    Serial.println (msg);
    return;
  }
  Serial.println("serialpoer test");
  if (msg.startsWith("[SC:DRIVERS:")){
    // 012345678901234567890
    // [SC:DRIVERS:FP:TT]
    displaybuffer[0]=msg.charAt(12);
    displaybuffer[1]=msg.charAt(13);
    displaybuffer[2]=msg.charAt(14);
    displaybuffer[3]=msg.charAt(15);
    Serial.print ("Drivers:");
    Serial.println (msg);
    Ddrivers();
  }
  if (msg.startsWith("[MX BLINK ON")){
    Serial.println ("Debug MX blink on");
  }
  if (msg=="[MX BLINK ON]"){
    Serial.println ("Debug MX blink on2");
    blink=true;
    ledState = LOW;
  }
  if (msg=="[MX BLINK OFF]"){
    blink=false;
    ledState = LOW;
  }
  if (msg=="[MX LED OFF]"){
    blink=false;
    ledState = LOW;
    neoYellow(3,3,true);
  }
  if (msg=="[MX LED ON]"){
    blink=false;
    ledState = HIGH;
    neoBlue(3,3,true);
  }
  if (msg=="[SC:SENSOR:1:0]") sensorDetect(0,0);
  if (msg=="[SC:SENSOR:1:1]") sensorDetect(0,1);
  if (msg=="[SC:SENSOR:2:0]") sensorDetect(1,0);
  if (msg=="[SC:SENSOR:2:1]") sensorDetect(1,1);
  if (msg=="[S:AC]"){
    Serial.println ("[S:AC:0]");
    Serial1.println ("[S:AC]");
  }
}
void sensorDetect(int track,int value){
  //long previousDetect0[]={0,0};
  previousSensor[track]=1; // temporary
  if (previousSensor[track]==1){
    if (value==1){
      sensorCnt[track]++;
      sensorCnt[track]=3;
    if (sensorCnt[track]==3){
      if (millis()>previousDetect1[track]+3000){
        SCdetect(track);
        previousDetect1[track]=millis();
      }
    }
    }
  }

}

void SCdetect(int track){
  
  if (track==1){
    Serial.println ("detect track 1");
  }
    if (track==2){
    Serial.println ("detect track 2");
  }
  raceHandler(1,track);
}

void raceHandler(int event,int track){
  if (RaceStatus=="FINISH"){
    Serial.print ("RACE STATUS : FINISH:");
    Serial.print (FinishTime[0]);
    Serial.print ("-");
    Serial.print (FinishTime[1]);
    Serial.println (":");
  }
  
  if (RaceStatus=="UPLOAD") Serial.println ("RACE STATUS : UPLOAD");
  //Serial.print ("RaceHandler:");
  //Serial.println (event);
    if (RaceStatus=="UPLOAD"){
      Serial.println ("UPLOAD not Implemented");
      delay(5000);
      RaceStatus="PREPARE";
    }
  if (event==1){
    if (RaceStatus=="START"){
      LapTime[0][0]=0;
      LapTime[1][0]=0;
      LapTime[0][1]=0;
      LapTime[1][1]=0;
      LapTime[0][2]=0;
      LapTime[1][2]=0;
      FinishTime[0]=0;
      FinishTime[1]=0;
      RaceStatus="RUN";
      RaceStartMillis=millis();
      updateDisplay();
      Serial.print ("[RACE:START:");
      Serial.print (RaceStartMillis);
      Serial.println ("]");
    }
   
    if (RaceStatus=="RUN"){
      int lapp=LapCount[track]-1;
      LapTime[track][lapp]=millis()-RaceStartMillis;
      boolean first=false;
      if (track==0) if (LapTime[1][lapp]==0) first=true;
      if (track==1) if (LapTime[0][lapp]==0) first=true;

      if (LapCount[track]==1){
        if (track==0){
          neoGreen(3,3,true);
          neoOff(2,4,true);

        }
        if (track==1){
          neoGreen(6,3,true);
          neoOff(7,4,true);
        }
      }
      if (LapCount[track]==3){
        RaceStatus="FINISH";
        FinishTime[track]=millis()-RaceStartMillis;
        if (track==0){
          if (FinishTime[1]==0){
            neoGreen(1,4,true);
            neoGreen(2,4,true);
            neoGreen(3,4,true);
          }
          else{
            neoYellow(1,4,true);
            neoYellow(2,4,true);
            neoYellow(3,4,true);
            
          }
        }
        if (track==1){
          if (FinishTime[0]==0){
            neoGreen(8,4,true);
            neoGreen(7,4,true);
            neoGreen(6,4,true);
          }
          else{
            neoYellow(8,4,true);
            neoYellow(7,4,true);
            neoYellow(6,4,true);
            
          }
        }
        updateDisplay();
        handleFinish();
      }
      else{
        LapCount[track]++;
        int y=1;
        if (LapCount[track]==1){
          y=1;
        }
        if (LapCount[track]==2){
          y=2;
        }
        if (LapCount[track]==3){
          y=3;
        }
        if (track==0){
          if (first) neoGreen(1,y,true);else neoYellow(1,y,true);
        }
        if (track==1){
          if (first) neoGreen(8,y,true);else neoYellow(8,y,true);
        }
        updateDisplay();
      }
    }
    return;
  }

  if (RaceStatus=="PREPARE"){
    //Serial.println ("debug PREPARE->COUNT");
    CountDownPreviousMillis = millis();
    RaceStatus="COUNT";
    CountDown=10;
    updateDisplay();
    //Serial.print ("Debug RaceStatus:");
    //Serial.println (RaceStatus);
    //while (true){}
  }
  else if (RaceStatus=="COUNT"){
    LapCount[0]=0;
    LapCount[1]=0;
    if (CountDown<1){
      Serial.println ("countdown<1: raceStatus-> Start");
      RaceStatus="START";
      neoOff(1,4,false);
      neoOff(8,4,true);
      neoGreen(2,4,false);
      neoGreen(7,4,true);

      RaceStartMillis=millis();
      updateDisplay();
    }
  }
  else if (RaceStatus=="START"){
    //if ((millis()-RaceStartMillis)>10000) test 20190131
    if ((millis()-RaceStartMillis)>20000)
    RaceStatus="PREPARE"; 
    updateDisplay();
  }
  // String RaceStatus="PREPARE";  // PREPARE,COUNT,START,RUN,FINISH
  
}
void handleFinish(){
  if (FinishTime[0]==0){
      RaceStatus="RUN";
      return;
  }
  if (FinishTime[1]==0){
      RaceStatus="RUN";
      return;
   }
    
        CountDownPreviousMillis = millis();
        RaceStatus="UPLOAD";
        Serial.print ("[RACEFINISH:");
        Serial.print (racenum);
        Serial.print (":START:");
        Serial.print (RaceStartMillis);
        Serial.print (racenum);
        Serial.print (":TRACK:1:");
        Serial.print (LapTime[0][0]);
        Serial.print (":");
        Serial.print (LapTime[0][1]);
        Serial.print (":");
        Serial.print (LapTime[0][2]);
        Serial.print (":FINISH:");
        Serial.print (FinishTime[0]);
        Serial.print (":TRACK:2:");
        Serial.print (LapTime[1][0]);
        Serial.print (":");
        Serial.print (LapTime[1][1]);
        Serial.print (":");
        Serial.print (LapTime[1][2]);
        Serial.print (":FINISH:");
        Serial.print (FinishTime[1]);
        Serial.println ("]");
        delay(5000);
        CountDown=20;
        updateDisplay();
}
void updateDisplay(){
  if (RaceStatus=="PREPARE"){
    Serial.println ("[PREPARE TO RACE  ");
  }
  if (RaceStatus=="COUNT"){
    Serial.print ("[TIME TO START ");
    Serial.println (CountDown);
    neoCount(CountDown);
  }
  if (RaceStatus=="START"){
    //Serial.print ("[RACE TIME ");
    //Serial.println (millis( )- RaceStartMillis);
    // DraceTime(int display,int ms,boolean ms)
    DraceTime(1,millis( )- RaceStartMillis,false);
  }
  if (RaceStatus=="RUN"){
    /*
    Serial.print ("[RACE TIME ");
    Serial.print (millis( )- RaceStartMillis);
    Serial.print ("LAP:");
    Serial.print (LapCount[0]);
    Serial.print (":");
    Serial.println (LapCount[1]);
    */
    DraceTime(1,millis( )- RaceStartMillis,false);

  }
  if(RaceStatus=="FINISH"){
    Serial.print("[FINISH TIME:");
    Serial.print(FinishTime[0]);
    Serial.print(":");
    Serial.print(FinishTime[1]);
    Serial.println("]");
    DraceTime(0,millis( )- RaceStartMillis,false);

  }
}
void neoCount(int c){
  if (c==10){
    neoAllOff();
  }
  if (c==9){
    neoRed(1,1,false);
    neoRed(1,2,false);
    neoRed(1,3,false);
    neoRed(1,4,false);
    neoRed(2,1,false);
    neoRed(3,1,false);
    neoRed(4,1,false);
    neoRed(5,1,false);
    neoRed(6,1,false);
    neoRed(7,1,false);
    neoRed(8,1,false);
    neoRed(8,2,false);
    neoRed(8,3,false);
    neoRed(8,4,true);
  }
  if (c==6){
    neoOff(4,1,false);
    neoOff(5,1,true);
  }
  if (c==5){
    neoOff(3,1,false);
    neoOff(6,1,true);
  }
  if (c==4){
    neoOff(2,1,false);
    neoOff(7,1,true);
  }
  if (c==3){
    neoOff(1,1,false);
    neoOff(8,1,true);
  }
  if (c==2){
    neoOff(1,2,false);
    neoOff(8,2,true);
  }
  if (c==1){
    neoOff(1,3,false);
    neoOff(8,3,true);
  }
}
void neoOff(byte x, byte y,boolean refresh){
  neoChange(x,y,0,0,0,refresh);
}
void neoRed(byte x, byte y,boolean refresh){
  neoChange(x,y,10,0,0,refresh);
}
void neoGreen(byte x, byte y,boolean refresh){
  neoChange(x,y,0,10,0,refresh);
}
void neoYellow(byte x, byte y,boolean refresh){
  neoChange(x,y,10,10,0,refresh);
}
void neoWhite(byte x, byte y,boolean refresh){
  neoChange(x,y,5,5,5,refresh);
}
void neoBlue(byte x, byte y,boolean refresh){
  neoChange(x,y,0,0,10,refresh);
}
void neoChange(byte x, byte y, byte r, byte g, byte b, boolean refresh){
  int i=x-1;
  y=y-1;
  i=i+(y*8);
   pixels.setPixelColor(i, pixels.Color(r,g,b));
   if (refresh) pixels.show(); // This sends the updated pixel color to the hardware.
}
void neoAllOff(){
  neoAll(0,0,0);
}
void neoAllWeak(){
  neoAll(10,10,10);
}
void neoAllRed(){
  neoAll(10,0,0);
}
void neoAllGreen(){
  neoAll(0,10,0);
}
void neoAllYellow(){
  neoAll(10,10,0);
}

void neoAll(byte r,byte g, byte b){
    for(int i=0;i<NUMPIXELS;i++){
        pixels.setPixelColor(i, pixels.Color(r,g,b));
    pixels.show(); // This sends the updated pixel color to the hardware.
  }
}
void DraceTime(int display,int ms,boolean displayMs){
  int sec=ms/1000;
  if (sec==raceSecPrev) return;
  raceSecPrev=sec;
  int d1=sec/10;
  if (d1>9){
  alpha1.writeDigitAscii(0, 'X');
  alpha1.writeDigitAscii(1, 'X');
  alpha1.writeDigitAscii(2, 'X');
  alpha1.writeDigitAscii(3, 'X');
 
  // write it out!
alpha1.writeDisplay();

    return;
  }
  int d2=sec-(d1*10);
  Serial.print("time:");
  Serial.print(d1);
  Serial.print(":");
  Serial.print(d2);
  Serial.println("");
  alpha1.writeDigitAscii(0, getChar2(d1));
  alpha1.writeDigitAscii(1, getChar2(d2));
  alpha1.writeDigitAscii(2, '-');
  alpha1.writeDigitAscii(3, '-');
 
  // write it out!
alpha1.writeDisplay();

  //delay(1000); 
}
char getChar2(int n){
  //int n=0;
  if (n==0) return '0';
  if (n==1) return '1';
  if (n==2) return '2';
  if (n==3) return '3';
  if (n==4) return '4';
  if (n==5) return '5';
  if (n==6) return '6';
  if (n==7) return '7';
  if (n==8) return '8';
  if (n==9) return '9';
}
void Ddrivers(){
  alpha4.writeDigitAscii(0, displaybuffer[0]);
  alpha4.writeDigitAscii(1, displaybuffer[1]);
  alpha4.writeDigitAscii(2, displaybuffer[2]);
  alpha4.writeDigitAscii(3, displaybuffer[3]);
 
  // write it out!
alpha4.writeDisplay();
Ddrivers2();
}
void Ddrivers2(){
  alpha1.writeDigitAscii(0, displaybuffer[0]);
  alpha1.writeDigitAscii(1, displaybuffer[1]);
  alpha1.writeDigitAscii(2, displaybuffer[2]);
  alpha1.writeDigitAscii(3, displaybuffer[3]);
 
  // write it out!
alpha1.writeDisplay();
}
//------------------------------------------------------------------------------------------
void HIuniversialInput(byte serialPort,char x){// 20181205 - copied from  HIclientInput0    //  20181006 . copied from 10.0.8 HandleClientInput0 
  if (msgUniversalInput[serialPort].length()>0){
    msgUniversalInput[serialPort]=msgUniversalInput[serialPort]+x;
    if (x==']'){                  // Detecting Proprietary Frontend, End of message.
      if (D)Serial.print(F("HandleUniversalInput:EOM"));
      HandleMessage(serialPort,msgUniversalInput[serialPort]);
      msgUniversalInput[serialPort]="";
    }
  }
  else if (msgUniversalInput[serialPort].length()==0){
    if (x=='['){                  // Detecting Proprietary Frontend, Start of message.
      msgUniversalInput[serialPort]="[";
    }
  }
}
