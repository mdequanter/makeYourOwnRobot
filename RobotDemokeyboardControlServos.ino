/*
In this tutorial we will build a robot that consists of 4 individually proportionally controlled servo motors.

Rotate Up and down Forward and backward Grab and release

We use Arduino as a microcontroller management platform. Through our computer, we can control the robot via serial commands (USB port).

The robot itself also has a recording and playback function so that repetitive tasks can be performed.

Moreover, the step size can be adjusted so that we can work with millimeter accuracy.

As hardware we use the technic blocks from the Heliwi.com construction kit.

This code can be uploaded to your Arduino Uno

#  Created by Maarten Dequanter (Erasmus Hogeschool Brussel) 29/11/2021

*/

#include <Servo.h>

Servo angle;
Servo up;
Servo grip;
Servo turn;

int posAngle = 90;
int posUp = 90;
int posGrip = 90;
int posTurn = 90;
int recordings[300];
int recordingType[300];

int stepSize = 10;




int recordingsPos = 0;
bool recording = false;

int receivedChar;
bool newData;

void setup() {
    resetRecordings();
    angle.attach(6);
    up.attach(9);
    grip.attach(10);
    turn.attach(11);
    Serial.begin(19200);
    Serial.println("<Arduino Robot is starting>");

    angle.write(posAngle);
    delay(15);
    up.write(posUp);
    delay(15);
    grip.write(posGrip);
    delay(15);
    turn.write(posTurn);

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

void resetOneServo(Servo *servoLink,int pos) {
  if (pos>90) {
    for (int i = pos; i>=90; i--) {
      servoLink->write(i);
      delay(50);
    }
  }
  if (pos<90) {
    for (int i = pos; i<=90; i++) {
      servoLink->write(i);
      delay(50);
    }
  }
}




void resetServos() {

        Servo *pServo;
        pServo = &turn;        
        resetOneServo(pServo,posTurn);
        pServo = &angle;        
        resetOneServo(pServo,posAngle);
        pServo = &up;        
        resetOneServo(pServo,posUp);
        pServo = &grip;        
        resetOneServo(pServo,posGrip);

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
        angle.write(recordings[i]);
        Serial.print("angle: ");
        Serial.println(recordings[i]);
        delay(150);
      }
      if (recordingType[i] == 9) {
        up.write(recordings[i]);
        Serial.print("up: ");
        Serial.println(recordings[i]);
        delay(150);
      }
      if (recordingType[i] == 10) {
        grip.write(recordings[i]);
        Serial.print("grip: ");
        Serial.println(recordings[i]);
        delay(150);
      }
      if (recordingType[i] == 11) {
        Serial.print("turn: ");
        Serial.println(recordings[i]);
        turn.write(recordings[i]);
        delay(150);
      }
      
    }
  }
  Serial.println("Done!");
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
          grip.write(posGrip);
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
          grip.write(posGrip);
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
          up.write(posUp);
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
          up.write(posUp);
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
          
          turn.write(posTurn);
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
          turn.write(posTurn);
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
          angle.write(posAngle);
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
          angle.write(posAngle);
          delay(15);
        }

        newData = false;
    }
}
