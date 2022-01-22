/*
In this tutorial we will build a robot that consists of 4 individually proportionally controlled servo motors.
Rotate Up and down Forward and backward Grab and release
We use Arduino as a microcontroller management platform. Through our computer, we can control the robot via serial commands (USB port).
The robot itself also has a recording and playback function so that repetitive tasks can be performed.
Moreover, the step size can be adjusted so that we can work with millimeter accuracy.
As hardware we use the technic blocks from the Heliwi.com construction kit.
This code can be uploaded to your Arduino Uno
#  Created by Maarten Dequanter (Erasmus Hogeschool Brussel) 29/11/2021
*
*
*/
#include <Adafruit_PWMServoDriver.h>
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define SERVOMIN  115 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  450 // This is the 'maximum' pulse length count (out of 4096)
#define USMIN  600 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  2400 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

uint8_t servonum = 0;

int posAngle = 90;
int posUp = 90;
int posGrip = 90;
int posTurn = 90;
int recordings[300];
int recordingType[300];


int ServoAngle = 0;
int ServoUp = 1;
int ServoGrip = 2;
int ServoTurn = 3;

int stepSize = 10;




int recordingsPos = 0;
bool recording = false;

int receivedChar;
bool newData;
int degreesVal;

int mapServo (int degreesVal) {
  return map(degreesVal, 0, 180, SERVOMIN, SERVOMAX);
}

void setup() {

    pwm.begin();
    pwm.setOscillatorFrequency(27000000);
    pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
  
    resetRecordings();
    
    Serial.begin(19200);
    Serial.println("<Arduino Robot is starting>");

    degreesVal = mapServo(90);


    pwm.setPWM(ServoAngle, 0, degreesVal );
    pwm.setPWM(ServoGrip, 0, degreesVal);
    pwm.setPWM(ServoUp, 0, degreesVal);
    pwm.setPWM(ServoTurn, 0, degreesVal);
    delay(300);
    Serial.println("<All servo in central position 90 degrees>");
}

void resetRecordings() {
  for (int i = 0 ; i<300; i++) {
    recordings[i] = 0;
  }
  recordingsPos = 0;
}

void loop() {
    recvOneChar();
    showNewData();
}

void recvOneChar() {
    if (Serial.available() > 0) {
        receivedChar = Serial.read();
        //Serial.println(receivedChar);
        newData = true;
    }
}

void resetOneServo(int servoLink,int pos) {
  
  if (pos>90) {
    for (int i = pos; i>=90; i--) {
      servoWrite(servoLink, pos);
      delay(50);
    }
  }
  if (pos<90) {
    for (int i = pos; i<=90; i++) {
      servoWrite(servoLink, pos);
      delay(50);
    }
  }
}




void resetServos() {

        resetOneServo(ServoTurn,posTurn);
        resetOneServo(ServoAngle,posAngle);
        resetOneServo(ServoUp,posUp);     
        resetOneServo(ServoGrip,posGrip);

        posTurn = 90;
        posAngle = 90;
        posUp =  90;
        posGrip = 90;
}

void playRecording() {
  Serial.println("Playing ....");
  for (int i = 0; i<300; i++) {
    if (recordings[i] > 0 ) {
      if (recordingType[i] == 6) {
        servoWrite(ServoAngle, recordings[i]);
        Serial.print("angle: ");
        Serial.println(recordings[i]);
        delay(150);
      }
      if (recordingType[i] == 9) {
        servoWrite(ServoUp, recordings[i]);
        Serial.print("up: ");
        Serial.println(recordings[i]);
        delay(150);
      }
      if (recordingType[i] == 10) {
        servoWrite(ServoGrip, recordings[i]);
        Serial.print("grip: ");
        Serial.println(recordings[i]);
        delay(150);
      }
      if (recordingType[i] == 11) {
        Serial.print("turn: ");
        Serial.println(recordings[i]);
        servoWrite(ServoTurn, recordings[i]);
        delay(150);
      }
      
    }
  }
  Serial.println("Done!");
}

void servoWrite(int servo,int value) {
  int servoValue = mapServo(value);
  pwm.setPWM(servo, 0, servoValue );
}


void showNewData() {
    if (newData == true) {
        //Serial.println(receivedChar);
        if (receivedChar == 112 and recording == false){
          playRecording();
        }

        if (recordingsPos > 299) {
            recording = false;
            Serial.println("Memory full: recording stopped");          
        }

        if (receivedChar == 102){
          stepSize-=1;
          if (stepSize <= 1) {
            stepSize = 1;
          }
          Serial.print("Stepsize:");  
          Serial.println(stepSize);  
        }
        
        if (receivedChar == 103){
          stepSize+=1;
          if (stepSize >= 10) {
            stepSize = 10;
          }
          Serial.print("Stepsize:");  
          Serial.println(stepSize);  
        }


        if (receivedChar == 114){
          if (recording == false) {
            recording = true;
            resetRecordings();
            Serial.println("recording started");
          }else { 
            recording = false;
            Serial.println("recording stopped");
          }
        }

        if (receivedChar == 32){
          resetServos();
        }
        

        if(receivedChar == 97){
          Serial.println("grip close");
          posGrip = posGrip+ stepSize;
          if (posGrip>180){
            posGrip = 180;
          }
          servoWrite(ServoGrip, posGrip);
          if (recording == true) {
            recordingType[recordingsPos] = 10;
            recordings[recordingsPos] = posGrip;
            recordingsPos++;
          }
          delay(15);
        }
        if(receivedChar == 101){
          Serial.println("grip open");
          posGrip = posGrip- stepSize;
          if (posGrip<=0){
            posGrip = 0;
          }
          if (recording == true) {
            recordingType[recordingsPos] = 10;
            recordings[recordingsPos] = posGrip;
            recordingsPos++;
          }
          servoWrite(ServoGrip, posGrip);
          delay(15);
        }
        if(receivedChar == 120){
          Serial.println("up");
          posUp = posUp+ stepSize;
          if (posUp>180){
            posUp = 180;
          }
          if (recording == true) {
            recordingType[recordingsPos] = 9;
            recordings[recordingsPos] = posUp;
            recordingsPos++;
          }
          servoWrite(ServoUp, posUp);
          delay(15);
        }
        if(receivedChar == 122){
          Serial.println("down");
          posUp = posUp- stepSize;
          if (posUp<=0){
            posUp = 0;
          }
          if (recording == true) {
            recordingType[recordingsPos] = 9;
            recordings[recordingsPos] = posUp;
            recordingsPos++;
          }
          servoWrite(ServoUp, posUp);
          delay(15);
        }
        if(receivedChar == 113){
          Serial.println("left");
          posTurn = posTurn+ stepSize;
          if (posTurn>180){
            posTurn = 180;
          }
          if (recording == true) {
            recordingType[recordingsPos] = 11;
            recordings[recordingsPos] = posTurn;
            recordingsPos++;
          }
          servoWrite(ServoTurn, posTurn);
          delay(15);
        }
        if(receivedChar == 100){
          Serial.println("right");
          posTurn = posTurn- stepSize;
          if (posTurn<=0){
            posTurn = 0;
          }
          if (recording == true) {
            recordingType[recordingsPos] = 11;
            recordings[recordingsPos] = posTurn;
            recordingsPos++;
          }
          servoWrite(ServoTurn, posTurn);
          delay(15);
        }
        if(receivedChar == 99){
          Serial.println("angle up");
          posAngle = posAngle+ stepSize;
          if (posAngle>180){
            posAngle = 180;
          }
          if (recording == true) {
            recordingType[recordingsPos] = 6;
            recordings[recordingsPos] = posAngle;
            recordingsPos++;
          }
          servoWrite(ServoAngle, posAngle);
          delay(15);
        }
        if(receivedChar == 119){
          Serial.println("angle down");
          posAngle = posAngle- stepSize;
          if (posAngle<=0){
            posAngle = 0;
          }
          if (recording == true) {
            recordingType[recordingsPos] = 6;
            recordings[recordingsPos] = posAngle;
            recordingsPos++;
          }
          servoWrite(ServoAngle, posAngle);
          delay(15);
        }

        newData = false;
    }
}
