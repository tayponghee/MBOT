#define TIMEOUT 2000 // Max microseconds to wait; choose according to max distance of wall
#define SPEED_OF_SOUND 340 // Update according to your own experiment
#define ULTRASONIC 12
#define IR_RECEIVE A2
#define IR_EMIT A3
#include "MeMCore.h"
#define TURNING_TIME_MS 330 // The time duration (ms) for turning
#define C4  262
#define F4  349
#define G4  392
#define A4  440
#define AS4 466
#define C5  523
#define D5  587
#define DS5 622
#define F5  698
#define G5  784
#define A5  880
#define AS5 932
#define C6  1047
MeDCMotor leftMotor(M1); // assigning leftMotor to port M1
MeDCMotor rightMotor(M2); // assigning RightMotor to port M2
MeLineFollower lineFinder(PORT_2);
MeBuzzer buzzer;
uint8_t motorSpeed = 230;
// Setting motor speed to an integer between 1 and 255
// The larger the number, the faster the speed
// Define time delay before the next RGB colour turns ON to allow LDR to stabilize
#define RGBWait 200 //in milliseconds

// Define time delay before taking another LDR reading
#define LDRWait 10 //in milliseconds

#define LDR 2   //LDR sensor pin at A0
#define LED 13  //Check Indicator to signal Calibration Completed

// Define colour sensor LED pins
//int ledArray[] = {2,3,4};

//placeholders for colour detected
int red = 0;
int green = 0;
int blue = 0;

//floats to hold colour arrays
float colourArray[] = {0,0,0};
float whiteArray[] = {13,19,12};
float blackArray[] = {3,3,2};
float greyDiff[] = {10,16,10};

char colourStr[3][5] = {"R = ", "G = ", "B = "};

int getAvgReading(int times) {
  //find the average reading for the requested number of times of scanning LDR
  int reading;
  int total =0;
  //take the reading as many times as requested and add them up
  for(int i = 0;i < times;i++){
    reading = analogRead(LDR);
    total = reading + total;
    delay(LDRWait);
  }
  //calculate the average and return it
  return total/times;
}

void setup() {
  // Any setup code here runs only once:
  Serial.begin(9600);
  delay(1000); // Do nothing for 1000 ms = 1 seconds
}

void loop() {
  pinMode(ULTRASONIC, OUTPUT);
  digitalWrite(ULTRASONIC, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC, LOW);
  pinMode(ULTRASONIC, INPUT);
  leftMotor.run(-motorSpeed); // Negative: wheel turns anti-clockwise
  rightMotor.run(motorSpeed); // Positive: wheel turns clockwise
  float duration = pulseIn(ULTRASONIC, HIGH, TIMEOUT);
  float distance_right = (duration / 2.0 / 1000000 * SPEED_OF_SOUND * 100); /** calculation of distance for ultrasonic sensor */
  Serial.println(distance_right);
  if(distance_right < 8.0 && distance_right > 0.1) { /** too close to right correct to the left, corrects more if the robot is closer to the wall */
    if(distance_right < 6.0 && distance_right > 0.1) {
      leftMotor.run(-140);
      rightMotor.run(motorSpeed);
      delay(15);
    }
    else if(distance_right < 7.0) {
      leftMotor.run(-160);
      rightMotor.run(motorSpeed);
      delay(15);
    }
    else if(distance_right < 7.5) {
      leftMotor.run(-190);
      rightMotor.run(motorSpeed);
      delay(15);
    }
    else {
      leftMotor.run(-215);
      rightMotor.run(motorSpeed);
      delay(15);
    }
  }
  float distance_left = analogRead(A3) / 19.382;
  if(distance_left <= 3.3)  { /** robot is too close to the left correct to the right, corrects more if the robot is closer to the wall */
    if(distance_left >= 2.7) {
      leftMotor.run(-motorSpeed);
      rightMotor.run(215);
      delay(15);
    }
    else if(distance_left >= 2.0) {
      leftMotor.run(-motorSpeed);
      rightMotor.run(190);
      delay(15);
    }
    else {
      leftMotor.run(-motorSpeed);
      rightMotor.run(160);
      delay(15);
    }
  }
  int sensorState = lineFinder.readSensors(); // read the line sensor's state
  if (sensorState == S1_IN_S2_IN) { // run colour sensor when mbot stops at blackline
    leftMotor.stop(); // Left wheel Stop
    rightMotor.stop(); // Right wheel stop
    delay(500);
    analogWrite(A0,255);
    analogWrite(A1,255); // Red LED lights up to give red value of rgb colour
    delay(200);
    colourArray[0] = getAvgReading(5);
    analogWrite(A0,255);
    analogWrite(A1,0); // Green LED lights up to give green value of rgb colour
    delay(200);
    colourArray[1] = getAvgReading(5);
    analogWrite(A0,0);
    analogWrite(A1,255); // Blue LED lights up to give blue value of rgb colour
    delay(200);
    colourArray[2] = getAvgReading(5);
    for (int c = 0; c <= 2; c++) {
      Serial.print(colourStr[c]);
      colourArray[c] = (colourArray[c] - blackArray[c]) / (greyDiff[c])*255; /** convert reading to scale of 255 */
      Serial.println(int(colourArray[c])); //show the value for the current colour LED,
    }
    if (colourArray[0] >= 200 && colourArray[1] >= 200){ // white sensed play music
      double k = 2.5;
      delay(10);

      for(int i=0; i<3; i+=1){
        buzzer.tone(A4, 250/k);
      }
      for(int i=0; i<3; i+=1){
        buzzer.tone(F5, 250/k);
      }
      for(int i=0; i<3; i+=1){
        buzzer.tone(G5, 250/k);
      }
      for(int i=0; i<3; i+=1){
        buzzer.tone(G5, 250/k);
      }
      for(int i=0; i<3; i+=1){
        buzzer.tone(G5, 250/k);
      }

      buzzer.tone(C4, 250/k);
      buzzer.tone(AS4, 250/k);
      buzzer.tone(A4, 250/k);
      buzzer.tone(F4, 250/k);
      for(int i =0; i<2; i+=1){
        buzzer.tone(G4, 500/k);
        buzzer.tone(G4, 250/k);
        buzzer.tone(D5, 250/k);
        buzzer.tone(C5, 500/k);
        buzzer.tone(AS4, 500/k);

        buzzer.tone(A4, 500/k);
        buzzer.tone(A4, 250/k);
        buzzer.tone(A4, 250/k);
        buzzer.tone(C5, 500/k);
        buzzer.tone(AS4, 250/k);
        buzzer.tone(A4, 250/k);

        for(int i =0; i<2; i+=1) {
          buzzer.tone(G4, 500/k);
          buzzer.tone(G4, 250/k);
          buzzer.tone(AS5, 250/k);
          buzzer.tone(A5, 250/k);
          buzzer.tone(AS5, 250/k);
          buzzer.tone(A5, 250/k);
          buzzer.tone(AS5, 250/k);
          buzzer.tone(A5, 250/k);
          buzzer.tone(AS5, 250/k);
        }
      }
    }
    else if (colourArray[0] > 180) { /** the value of colourArray[0], which is red, would be above this threshold if the papers are red or orange */
      if(colourArray[1] >= 95) {
        leftMotor.run(150);
        rightMotor.run(150);
        delay(1000);
        leftMotor.run(0);
        rightMotor.run(0);
        delay(50);
        //do orange;
      } else {
        leftMotor.run(150);
        rightMotor.run(150);
        delay(500);
        leftMotor.run(0);
        rightMotor.run(0);
        delay(50);
        // do red;
      }
    }
    else if(colourArray[0] > 76 && colourArray[2] >= 153) { /** the value of colourArray[0], which is red, would be above this threshold if the papers are blue or purple */
      if(colourArray[1]>= 127) {
        leftMotor.run(-150);
        rightMotor.run(-150);
        delay(500);
        leftMotor.run(-150);
        rightMotor.run(150);
        delay(1100);
        leftMotor.run(-150);
        rightMotor.run(-150);
        delay(500);
        leftMotor.run(0);
        rightMotor.run(0);
        delay(50);
        //do blue;
      } else {
        leftMotor.run(150);
        rightMotor.run(150);
        delay(500);
        leftMotor.run(-150);
        rightMotor.run(150);
        delay(1100);
        leftMotor.run(150);
        rightMotor.run(150);
        delay(500);
        leftMotor.run(0);
        rightMotor.run(0);
        delay(50);
        // do purple;
      }
    }
    else {
      leftMotor.run(-150);
      rightMotor.run(-150);
      delay(500);
      leftMotor.run(0);
      rightMotor.run(0);
      delay(50);
      //do green;
    }
  }
}
