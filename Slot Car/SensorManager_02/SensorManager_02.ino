//--------------------------------------------------------------------------------
//[SC:SENSOR:1:1]
// SC030003 - Standalone on Arduino UNO
// SC030004 - HW test : Blink on Arduino, output on 8 and 12
// RCSensorMaanger2 -  Believed to be current sw on Arduino Slaves
// 20190203 - SCSlaveManager01
// 20190205 - SCSlaveManager02 - Neopixel
//SensorManager_02 - Trying to clean up by removing stuff with LEDs

//--------------------------------------------------------------------------------
#include <Adafruit_NeoPixel.h>
// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
//#define PIN            6
#define PIN            12

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      4

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
boolean HWtestNEO=false;
boolean SlaveRaceStarted=false;
//--------------------------------------------------------------------------------
// Configuration variables:
//-------------------------
boolean SlaveNode=true;
boolean blink=false;
boolean SlowBlink=false;

boolean BlinkOutput=false;                 // Simulate sensors - by blinking
boolean sensorsOnly=true;                 // not used
boolean enableSerial=true;
char NodeId='2';
//---------------------------------------------------------------------------------
// DEBUG :
//---------
boolean DebugBlink=true;                  // Blink leds, and outputs
boolean DebugSerialChar=false;
boolean DS=false;
boolean D=false;

//---------------------------------------------------------------------------------
// constants won't change. Used here to set pin numbers:
  // Pin 13: Arduino has an LED connected on pin 13
  // Pin 11: Teensy 2.0 has the LED on pin 11
  // Pin  6: Teensy++ 2.0 has the LED on pin 6
  // Pin 13: Teensy 3.0 has the LED on pin 13
const int ledPin =  13;      // the number of the LED pin

// Variables will change:
int ledState = LOW;             // ledState used to set the LED
long previousMillis = millis();        // will store last time LED was updated
long LocalRaceStart=0;
int LocalTrackCounter[]={0,0};
long SetupLimits[4][3][5];

// the follow variables is a long because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long interval = 1000;           // interval at which to blink (milliseconds)
  unsigned long currentMillis = millis();

String RaceStatus="PREPARE";  // PREPARE,COUNT,START,RUN,FINISH
String TrackStatus[]={"PREPARE","PREPARE"};
String SensorStatus[]={"NO","NO","NO","NO"};
String msgUniversalInput[]={"","","",""};
int sensorCnt[]={0,0,0,0};
int CountDown=10;
long CountDownPreviousMillis = millis();        // will store last time LED was updated
long RaceStartMillis = millis();        // will store last time LED was updated
long FinishTime[]={0,0,0,0};
int LapCount[]={0,0,0,0};
long previousDetect0[]={0,0,0,0};
long previousDetect1[]={0,0,0,0};
int previousSensor[]={0,0,0,0};
byte sensorPin[]={2,4,16,17};  // 2 and 4 for Node 0:  14,15,16,17 (a0-a3) for Slave Node
byte OutputPin[]={8,12};  // not used on Slave node (maybe 12 for NeOPIXel ?
byte PixelStatusClass[4];
boolean PixelStatusLock[4];
// constants won't change. They're used here to 
// set pin numbers:
const int buttonPin = 2;     // the number of the pushbutton pin
//const int ledPin =  13;      // the number of the LED pin

// Variables will change:
//int ledState2 = HIGH;         // the current state of the output pin
int buttonState[]={0,0,0,0};           // the current reading from the input pin
int buttonStateOutOld[] = {0,0,0,0};
int lastButtonState[] = {LOW,LOW,LOW,LOW};   // the previous reading from the input pin

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime[] = {0,0,0,0};  // the last time the output pin was toggled
//long debounceDelayOn = 5;    // the debounce time; increase if the output flickers
long debounceDelayOn = 3;    // the debounce time; increase if the output flickers
long debounceDelayOff = 1000;    // the debounce time; increase if the output flickers



void setup() {
  setupNeo();
  if (SlaveNode) setupSlaveNode(); else setupMasterNode();
}
void setupMasterNode(){
  //pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(OutputPin[0], OUTPUT);
  pinMode(OutputPin[1], OUTPUT);
  openSerial();
}
void setupSlaveNode(){
  pinMode(ledPin, OUTPUT);
  //byte sensorPin[]={2,4,16,17};  // 2 and 4 for Node 0:  14,15,16,17 (a0-a3) for Slave Node
  sensorPin[0]=14;
  sensorPin[1]=15;              //TRENGER VI Å DEFINERE DETTE?
  openSerial();
}
void setupNeo(){
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
  neoAllRed();
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
void hwmonitor(){
    if (digitalRead(sensorPin[0])==HIGH) neoBlue(0,0,true); else neoWhite(0,0,true);  // 20190205, changed indexing for Slave
    if (digitalRead(sensorPin[1])==HIGH) neoBlue(0,1,true); else neoWhite(0,1,true);  // 20190205, changed indexing for Slave
    if (digitalRead(sensorPin[2])==HIGH) neoBlue(1,0,true); else neoWhite(1,0,true);  // 20190205, added 3rd input
    if (digitalRead(sensorPin[3])==HIGH) neoBlue(1,1,true); else neoWhite(1,1,true);  // 20190205, added fourth input
}
void loopPixel(){
  if (SlaveRaceStarted){
    long currentRaceTime=millis()-LocalRaceStart;
    if (currentRaceTime<400) neoAllOff(); 
    else if (currentRaceTime<800){
      neoAllBlue();
      PixelStatusClass[0]=0;
      PixelStatusClass[1]=0;
      PixelStatusClass[2]=0;
      PixelStatusClass[3]=0;
    }
    else{
      if (PixelStatusLock[0]==false) updatePixel2(0,0,currentRaceTime);  //  add other sensosrs / pixels 20190205
    }
    
  }
}
void updatePixel2(byte group,byte track,long raceTime){
    byte ix=group*2+track;

  // long getLimit(int sensorGroup,int trackId,int lap,int classification)
  int locallap=LocalTrackCounter[track];
  locallap--;
  if (locallap<0) locallap=0;
  if (raceTime> getLimit(group,track,locallap,5)){
    if ( PixelStatusClass[ix]!=5){
      neoViolet(group, track,true);
      PixelStatusClass[ix]=5;
    }    
  } 
  else if (raceTime> getLimit(group,track,locallap,4)){
    if ( PixelStatusClass[ix]!=4){
      neoRed(group, track,true);
      PixelStatusClass[ix]=4;
    }    
  } 
  else if (raceTime> getLimit(group,track,locallap,3)){
    if ( PixelStatusClass[ix]!=3){
      neoOrange(group, track,true);
      PixelStatusClass[ix]=3;
    }    
  } 
  else if (raceTime> getLimit(group,track,locallap,2)){
    if ( PixelStatusClass[ix]!=2){
      neoYellow(group, track,true);
      PixelStatusClass[ix]=2;
    }    
  } 
  else if (raceTime> getLimit(group,track,locallap,1)){
    if ( PixelStatusClass[ix]!=1){
      neoGreen(group, track,true);
      PixelStatusClass[ix]=1;
    }    
  } 
}

void updatePixel(byte group,byte track,long raceTime){
  byte ix=group*2+track;
  if (raceTime>20000){
    if ( PixelStatusClass[ix]!=5){
      neoViolet(group, track,true);
      PixelStatusClass[ix]=5;
    }
  }
  else if (raceTime>8000){
    if ( PixelStatusClass[ix]!=4){
      neoRed(group, track,true);
      PixelStatusClass[ix]=4;
    }
  }
  else if (raceTime>6000){
    if ( PixelStatusClass[ix]!=3){
      neoOrange(group, track,true);
      PixelStatusClass[ix]=3;
    }
  }
  else if (raceTime>2000){
    if ( PixelStatusClass[ix]!=2){
      neoYellow(group, track,true);
      PixelStatusClass[ix]=2;
    }
  }
  else if (raceTime>1000){
    if ( PixelStatusClass[ix]!=1){
      neoGreen(group, track,true);
      PixelStatusClass[ix]=1;
    }
  }
}

void loop(){
  if (SlaveNode)handleSerial();
  if (SlaveNode==false) pinControl();
  //if (SlaveNode) loopPixel();           //TEST
  long t1=micros();
  sensorHandler(0);
  long t2=micros();
  sensorHandler(1);
  long t3=micros();
  
  if (SlaveNode){
    sensorHandler(2);
    sensorHandler(3);
  }
  
/*                                        //TEST
  if (blink) refreshLed();
//  if (sensorsOnly==false) SCupdateTimers();
  long t4=micros();

//  if (sensorsOnly==false) raceHandler(0,0);
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
  } */
}

void sensorHandler(int trackid){  //trackid/sensor id :0-4
  // read the state of the switch into a local variable:
  int reading = digitalRead(sensorPin[trackid]);
  if (reading==HIGH) reading=LOW; else reading=HIGH;

  // check to see if you just pressed the button 
  // (i.e. the input went from LOW to HIGH),  and you've waited 
  // long enough since the last press to ignore any noise:  

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState[trackid]) {
    Serial.print ("debug sensorhandler,change  for sensor:");
    Serial.println (trackid);
    Serial.print ("PIN:");
    Serial.println (sensorPin[trackid]);
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
  //digitalWrite(ledPin, buttonState[trackid]);

  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState[trackid] = reading;
}
void handleSensorChange(int sensor, int value){  // sensor :0-4
  if (SensorStatus[sensor]=="NO"){
    if (value==1){
      if (SlaveNode){
        PixelStatusLock[sensor]=true;

          Serial.print ("Debug handlesensorchange, for sensor:");
          Serial.println (sensor);

        Serial.print("[NODE:");
        Serial.print(NodeId);
        Serial.print(":SENSOR:");
        Serial.print(sensor+1);
        Serial.println("]");
      }
      else{
        Serial.print ("TRACK:");
        Serial.print (sensor);
        Serial.print (",State:");
        Serial.println (value);
      }
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
void pinControl(){
  boolean ledOutput=false;
  if (ledState==1) ledOutput=true;
  if (buttonStateOutOld[0]!= buttonState[0]){
    if (buttonState[0]==HIGH){
      ledOutput=true;
      digitalWrite(OutputPin[0], HIGH);
    }
    else{
      digitalWrite(OutputPin[0], LOW);    
    }
    buttonStateOutOld[0]=buttonState[0];    
  }
  if (buttonStateOutOld[1]!= buttonState[1]){
    if (buttonState[1]==HIGH){
      ledOutput=true;
      digitalWrite(OutputPin[1], HIGH);
    }
    else{
      digitalWrite(OutputPin[1], LOW);
    }
    buttonStateOutOld[1]=buttonState[1];    
  }
  digitalWrite(ledPin, ledState);
  if (BlinkOutput){
    digitalWrite(OutputPin[0], ledState);
    digitalWrite(OutputPin[1], ledState); 
  }
}
//--------------------------------------------------------------------
// LED Control
//------------
void ledTogle(){
  if (DebugBlink) Serial.println ("DebugBlink:");
    if (ledState == LOW)
      LedControl(true);
    else
      LedControl(false);
}
void refreshLed(){
  int interval2=interval;
  if (ledState == LOW){
    if (SlowBlink) interval2=10000;
  }
  currentMillis = millis();
  if(currentMillis - previousMillis > interval2) {
    
    ledTogle(); 
    previousMillis = currentMillis;   
  }
  
}
void LedControl(boolean on){
  if (on) ledState = HIGH; else ledState = LOW;
  previousMillis = currentMillis;   
}
//--------------------------------------------------------------------
// Serial Debug
//-------------
void openSerial(){
//  Serial.begin(230400);
//  Serial.begin(115200);
  if (enableSerial) Serial.begin(9600);
  //delay(5000);
}
void handleSerial(){
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
  HIuniversialInput(sourcePort,x);
}
char getChar(int sp){
  if (sp==0) return Serial.read(); 
//  if (sp==1) return Serial1.read(); 
  return ' ';
}
boolean CharAvailable(int sp){
  boolean avail=false;
  if (sp==0){
    if (Serial.available()>0) avail=true;
  }
//  if (sp==1){
//    if (Serial1.available()>0) avail=true;
//  }
  return avail;
}
void SCdetect(int track){
  
  if (track==0){
    Serial.println ("detect track 0");
  }
  if (track==1){
    Serial.println ("detect track 1");
  }
    if (track==2){
    Serial.println ("detect track 2");
  }
    if (track==3){
    Serial.println ("detect track 3");
  }
//  raceHandler(1,track);
}

void HandleMessage(byte serialPort,String msg){
    boolean changedNodeId=false;

  //Serial.println ("Handle Message:"+msg);
  if (msg=="[MX BLINK ON]"){
    blink=true;
    ledState = LOW;
    Serial.println ("[MX BLINK ON]");
  }
  else if (msg=="[MX BLINK OFF]"){
    blink=false;
    ledState = LOW;
    Serial.println ("[MX BLINK OFF]");
  }
  else if (msg=="[MX LED OFF]"){
    blink=false;
    ledState = LOW;
    Serial.println ("[MX BLINK OFF]");
  }
  else if (msg=="[MX LED ON]"){
    blink=false;
    ledState = HIGH;
    Serial.println ("[MX BLINK ON]");
  }
  else if (msg=="[SC:SENSOR:1:0]") sensorDetect(0,0);
  else if (msg=="[SC:SENSOR:1:1]") sensorDetect(0,1);
  else if (msg=="[SC:SENSOR:2:0]") sensorDetect(1,0);
  else if (msg=="[SC:SENSOR:2:1]") sensorDetect(1,1);
  else if (msg=="[SC:SENSOR:3:0]") sensorDetect(2,0);
  else if (msg=="[SC:SENSOR:3:1]") sensorDetect(2,1);
  
  else if (msg=="[S:AC:1]"){
    NodeId='1';
    changedNodeId=true;
    Serial.println ("[S:AC:2]");
  }
  else if (msg=="[S:AC:2]"){
    NodeId='2';
    changedNodeId=true;
    Serial.println ("[S:AC:3]");
  }
  else if (msg=="[S:AC:3]"){
    NodeId='3';
    changedNodeId=true;
    Serial.println ("[S:AC:4]");
  }
  else if (msg=="[S:AC:4]"){
    NodeId='4';
    changedNodeId=true;
    Serial.println ("[S:AC:5]");
  }
  else if (msg=="[S:AC:4]"){
    NodeId='4';
    changedNodeId=true;
    Serial.println ("[S:AC:5]");
  }
  else if (msg=="[RACE:START]"){
    LocalRaceStart=millis();
    LocalTrackCounter[0]=0;
    LocalTrackCounter[1]=0;
    PixelStatusLock[0]=false;
    PixelStatusLock[1]=false;
    PixelStatusLock[2]=false;
    PixelStatusLock[3]=false;
    SlaveRaceStarted=true;
    Serial.println ("[RACE:START]");
  }
  else if (msg=="[TRACK:1:FIRST]"){
    LocalTrackCounter[0]=1;
    Serial.println ("[TRACK:1:FIRST]");
    //resetPixelModes0();  // not for start/first sensor only second
  }
  else if (msg=="[TRACK:2:FIRST]"){
    LocalTrackCounter[1]=1;
    Serial.println ("[TRACK:2:FIRST]");
    resetPixelModes1();
  }
  else if (msg=="[TRACK:1:SECOND]"){
    LocalTrackCounter[0]=2;
    Serial.println ("[TRACK:1:SECOND]");
    resetPixelModes0();
  }
  else if (msg=="[TRACK:2:SECOND]"){
    LocalTrackCounter[1]=2;
    Serial.println ("[TRACK:2:SECOND]");
    resetPixelModes1();
  }
  else if (msg=="[TRACK:1:THIRD]"){
    LocalTrackCounter[0]=3;
    Serial.println ("[TRACK:1:THIRD]");
    resetPixelModes0();
  }
  else if (msg=="[TRACK:2:THIRD]"){
    LocalTrackCounter[1]=3;
    Serial.println ("[TRACK:2:THIRD]");
    resetPixelModes1();
  }
  //                      [SETUP:node:sensorGroup:track:lap:classification:ms]
  //                      012345678901234567890123
  //                      [SETUP:1:0:0:1:0:000000]
  else if (msg.startsWith("[SETUP:")){
    SlaveRaceStarted=false;
    setupNode(msg);
  }
  else if (msg=="[TESTSETUP]"){
    testLimits();
  }
  else if (msg=="[DUMPSETUP]"){
    SlaveRaceStarted=false;  // temporary solution
    printLimits();
  }
  else Serial.println (msg);
  if (changedNodeId){
    neoAllOff();
    delay(1000);
    neoAllYellow();
    delay(1000);
    if (NodeId>=1) neoWhite(0,0,true);
    if (NodeId>=2) neoWhite(0,1,true);
    if (NodeId>=3) neoWhite(1,0,true);
    if (NodeId>=4) neoWhite(0,1,true);
    delay(1000);
    neoAllRed();
    delay(1000);
    neoAllBlue();
  }

}
void resetPixelModes(){
  resetPixelModes0();
  resetPixelModes1();
}
void resetPixelModes0(){
  neoAllBlue();
  PixelStatusLock[0]=false;
  PixelStatusLock[2]=false;
  PixelStatusClass[0]=0;
  PixelStatusClass[2]=0;
}
void resetPixelModes1(){
  neoAllBlue();
  PixelStatusLock[1]=false;
  PixelStatusLock[3]=false;
  PixelStatusClass[1]=0;
  PixelStatusClass[3]=0;
}
void setupNode(String msg){
  int msgLen=msg.length();
  if (msgLen!=24){
    Serial.println ("[ERROR LENGTH SETUP MESSAGE]");
    return;
  }
  Serial.print("Setup message length:");
  Serial.println (msgLen);
  char node=msg.charAt(7);
  char sensorGroup=msg.charAt(9);
  char trackId=msg.charAt(11);
  char lap=msg.charAt(13);
  char classification=msg.charAt(15);
  String mss=msg.substring(17,23);
  Serial.print ("timer value:");
  Serial.println (mss);
  long newTime=mss.toInt();
  Serial.print("timer2 value:");
  Serial.println (newTime);
  if (node!=NodeId){
    Serial.print(msg);
    return;
  }
  int ix=0;
  if (sensorGroup=='1') ix=2;
  if (trackId=='1') ix=ix+1;
  int lapix=0;
  if (lap=='2') lapix=1;
  if (lap=='3') lapix=2;
  int classix=0;
  if (classification=='1') classix=1;
  if (classification=='2') classix=2;
  if (classification=='3') classix=3;
  if (classification=='4') classix=4;
  if (classification=='5') classix=5;
  SetupLimits[ix][lapix][classix]=newTime;
}

long getLimit(int sensorGroup,int trackId,int lap,int classification){
  int ix=0;
  if (sensorGroup==1) ix=2;
  if (trackId==1) ix=ix+1;
  /*
  Serial.println ("");
  Serial.print(F("getLimit:sensorgroup:"));
  Serial.print(sensorGroup);
  Serial.print(F(":trackid:"));
  Serial.print(trackId);
  Serial.print(F(":ix:"));
  Serial.print(ix);
  Serial.print(":");
  */
  return (SetupLimits[ix][lap][classification]);
}

void printLimits(){
  
  Serial.println (F("============================================================"));
  Serial.println (F("PrintLimits:"));
  Serial.println (F("------------"));
  for (int i=0; i <= 1; i++){
    for (int j=0; j <= 1; j++){
      for (int k=0; k <= 2; k++){
        for (int l=0; l <= 5; l++){
    Serial.print(F("Sensor Group:"));
    Serial.print(i);
    Serial.print(F(", Track ID:"));
    Serial.print(j);
    Serial.print(F(", LAP:"));
    Serial.print(k+1);
    Serial.print(F(", Class:"));
    Serial.print(l);
    Serial.print(":");
    // long getLimit(int sensorGroup,int trackId,int lap,int classification){

    Serial.println(getLimit(i,j,k,l));
        }
      }
    }
  }
  Serial.println (F("============================================================"));
}
void testLimits(){
  Serial.println ("testLimits");
    long newtime=1;
    for (int i=0; i <= 1; i++){
    for (int j=0; j <= 1; j++){
      for (int k=0; k <= 2; k++){
        for (int l=0; l <= 5; l++){

    Serial.print(F("Sensor Group:"));
    Serial.print(i);
    Serial.print(F(", Track ID:"));
    Serial.print(j);
    Serial.print(F(", LAP:"));
   Serial.print(k+1);
    Serial.print(F(", Class:"));
    Serial.print(l);
    Serial.print(":");
   // Serial.println(getLimit(i,j,k,l));
    SetupLimits[i*2+j][k][l]=newtime;
    Serial.println(newtime);
   //
    newtime++;
        }
      }
    }
  }
  Serial.println ("testLimits End");
}


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
//-----------------------------------------------------------------------------------------
// NeoLibrary functions :
//------------------------
void neoOff(byte x, byte y,boolean refresh){
  neoChange(x,y,0,0,0,refresh);
}
void neoRed(byte x, byte y,boolean refresh){
  neoChange(x,y,10,5,0,refresh);
}
void neoOrange(byte x, byte y,boolean refresh){
  neoChange(x,y,10,0,0,refresh);
}
void neoViolet(byte x, byte y,boolean refresh){
  neoChange(x,y,10,0,10,refresh);
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
/*
void neoChange(byte x, byte y, byte r, byte g, byte b, boolean refresh){
  int i=x-1;
  y=y-1;
  i=i+(y*8);
   pixels.setPixelColor(i, pixels.Color(r,g,b));
   if (refresh) pixels.show(); // This sends the updated pixel color to the hardware.
}
*/
void neoChange(byte group, byte track, byte r, byte g, byte b, boolean refresh){
  int i=group*2;
  i=i+track;;
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
void neoAllBlue(){
  neoAll(0,0,10);
}
void neoAll(byte r,byte g, byte b){
    for(int i=0;i<NUMPIXELS;i++){
        pixels.setPixelColor(i, pixels.Color(r,g,b));
    pixels.show(); // This sends the updated pixel color to the hardware.
  }
}
