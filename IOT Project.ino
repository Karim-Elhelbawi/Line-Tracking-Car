/**********************************************************************
  Filename    : Tracking_Car.ino
  Product     : Freenove 4WD Car for ESP32
  Auther      : www.freenove.com
  Modification: 2020/12/18
**********************************************************************/

#include <Arduino.h>
#include <FirebaseESP32.h>
#include "Freenove_4WD_Car_For_ESP32.h"
#include <Wifi.h>

#define SPEED_LV4   ( 4000 )
#define SPEED_LV3   ( 3000 )
#define SPEED_LV2   ( 2500 )
#define SPEED_LV1   ( 1600 )


/*
Fix for turning problem, shift forward then turn slightly assuring u r in the white then rotate until u hit a black line
*/

const char *ssid = "Yh";
const char *password = "12309876";
const char *host = "embedded-project-ea0b2-default-rtdb.firebaseio.com";
const char *databaseSecret = "LXdpKZbG4nt0AyNMDm4DXF0GexDSnsAjwxFHapk2";

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;



int ball1X;
int ball1Y;
int ball2X;
int ball2Y;
int ball3X;
int ball3Y;
int myX = 0;
int myY = 0;
bool begin = true;
bool end = false;
bool last1 = true;
bool last2 = true;
bool last3 = true;
bool up = false;
bool down = false;
bool left = false;
bool right = true;
int goFirstX = 0;
int goFirstY = 0;
int goSecondX = 0;
int goSecondY = 0;
int goThirdX = 0;
int goThirdY = 0;
bool finish = false;






void fetchBallCoordinates() {
  Firebase.getString(firebaseData, "/ball1/X");
  ball1X = firebaseData.to<int>();
  Serial.print("Ball 1 X: ");
  Serial.println(ball1X);

  Firebase.getString(firebaseData, "/ball1/Y");
  ball1Y = firebaseData.to<int>();
  Serial.print("Ball 1 Y: ");
  Serial.println(ball1Y);

  Firebase.getString(firebaseData, "/ball2/X");
  ball2X = firebaseData.to<int>();
  Serial.print("Ball 2 X: ");
  Serial.println(ball2X);

  Firebase.getString(firebaseData, "/ball2/Y");
  ball2Y = firebaseData.to<int>();
  Serial.print("Ball 2 Y: ");
  Serial.println(ball2Y);

  Firebase.getString(firebaseData, "/ball3/X");
  ball3X = firebaseData.to<int>();
  Serial.print("Ball 3 X: ");
  Serial.println(ball3X);

  Firebase.getString(firebaseData, "/ball3/Y");
  ball3Y = firebaseData.to<int>();
  Serial.print("Ball 3 Y: ");
  Serial.println(ball3Y);
}


void calculations(){

  // Calculate distances to each ball from the starting position
  int distanceToBall1 = abs(ball1X - 0) + abs(ball1Y - 0);
  int distanceToBall2 = abs(ball2X - 0) + abs(ball2Y - 0);
  int distanceToBall3 = abs(ball3X - 0) + abs(ball3Y - 0);

  int basketToBall1 = abs(ball1X - 3) + abs(ball1Y - 3);
  int basketToBall2 = abs(ball2X - 3) + abs(ball2Y - 3);
  int basketToBall3 = abs(ball3X - 3) + abs(ball3Y - 3);

  // Determine the order to visit the balls
  int closestBallToStart = min(distanceToBall1, min(distanceToBall2, distanceToBall3));
  int closestBallToBasket = min(basketToBall1, min(basketToBall2, basketToBall3));

   if (closestBallToStart == distanceToBall1){
    goFirstX = ball1X;
    goFirstY = ball1Y;
    last1 = false;
  }

   else if (closestBallToStart == distanceToBall2){
    goFirstX = ball2X;
    goFirstY = ball2Y;
    last2 = false;
  }

   else {
    goFirstX = ball3X;
    goFirstY = ball3Y;
    last3 = false;
  }



  if (closestBallToBasket == basketToBall1){
    goSecondX = ball1X;
    goSecondY = ball1Y;
    last1 = false;
  }

  else if (closestBallToBasket == basketToBall2){
    goSecondX = ball2X;
    goSecondY = ball2Y;
    last2 = false;
  }

  else {
    goSecondX = ball3X;
    goSecondY = ball3Y;
    last3 = false;
  }



  if ((!last1) && (!last2)){
    goThirdX = ball3X;
    goThirdY = ball3Y;
  }

  else if ((!last1) && (!last3)){
    goThirdX = ball2X;
    goThirdY = ball2Y;
  }

  else {
    goThirdX = ball1X;
    goThirdY = ball1Y;
  }

}

void recoil(){
  Track_Read();

    Motor_Move(1500, 1500, 1500, 1500); //Move backward
    delay(185);
    Motor_Move(0, 0, 0, 0);
    delay(1500);

  }



void adjust(){
  Track_Read();
  
  while (sensorValue[3] == 5){
    Track_Read();
    if (sensorValue[3] == 5){
    Motor_Move(-1500, -1500, -1500, -1500); //Move Forward
    delay(1500);
  }
  }
}


void followLineX(){
  bool x = false;
    
  while (!x){
    Track_Read();

  switch (sensorValue[3])
  {
    case 2:   //010
    case 7:   //111
      Motor_Move(-SPEED_LV1, -SPEED_LV1, -SPEED_LV1, -SPEED_LV1); //Move Forward
      break;
    case 0:   //000
    case 5:   //101
      Motor_Move(0, 0, 0, 0);
      myX += 1;
      x = true;                              //Stop
      break;
    case 4:   //100
    case 6:   //110
      Motor_Move(-SPEED_LV1, -SPEED_LV1 , SPEED_LV1, SPEED_LV1); //Turn Right
      break;
    case 1:   //001
    case 3:   //011
      Motor_Move(SPEED_LV1, SPEED_LV1, -SPEED_LV1, -SPEED_LV1);  //Turn Left
      break;
    default:
      break;
  }
  }
  delay(1500);
}

void followLineNegX(){
  bool x = false;
    
  while (!x){
    Track_Read();

  switch (sensorValue[3])
  {
    case 2:   //010
    case 7:   //111
      Motor_Move(-SPEED_LV1, -SPEED_LV1, -SPEED_LV1, -SPEED_LV1); //Move Forward
      break;
    case 5:   //101
    case 0:   //000
      Motor_Move(0, 0, 0, 0);
      myX -= 1;
      x = true;                              //Stop
      break;
    case 4:   //100
    case 6:   //110
      Motor_Move(-SPEED_LV1, -SPEED_LV1 , SPEED_LV1, SPEED_LV1); //Turn Right
      break;
    case 1:   //001
    case 3:   //011
      Motor_Move(SPEED_LV1, SPEED_LV1, -SPEED_LV1, -SPEED_LV1);  //Turn Left
      break;
    default:
      break;
  }
  }
  delay(1500);
}

void followLineY(){
  bool x = false;
    
  while (!x){
    Track_Read();

  switch (sensorValue[3])
  {
    case 2:   //010
    case 7:   //111
      Motor_Move(-SPEED_LV1, -SPEED_LV1, -SPEED_LV1, -SPEED_LV1); //Move Forward
      break;
    case 0:   //000
    case 5:   //101
      Motor_Move(0, 0, 0, 0);
      myY += 1;
      x = true;                              //Stop
      break;
    case 4:   //100
    case 6:   //110
      Motor_Move(-SPEED_LV1, -SPEED_LV1 , SPEED_LV1, SPEED_LV1); //Turn Right
      break;
    case 1:   //001
    case 3:   //011
      Motor_Move(SPEED_LV1, SPEED_LV1, -SPEED_LV1, -SPEED_LV1);  //Turn Left
      break;
    default:
      break;
  }
  }
  delay(1500);
}

void followLineNegY(){
  bool x = false;
    
  while (!x){
    Track_Read();

  switch (sensorValue[3])
  {
    case 2:   //010
    case 7:   //111
      Motor_Move(-SPEED_LV1, -SPEED_LV1, -SPEED_LV1, -SPEED_LV1); //Move Forward
      break;
    case 0:   //000
    case 5:   //101
      Motor_Move(0, 0, 0, 0);
      myY -= 1;
      x = true;                              //Stop
      break;
    case 4:   //100
    case 6:   //110
      Motor_Move(-SPEED_LV1, -SPEED_LV1 , SPEED_LV1, SPEED_LV1); //Turn Right
      break;
    case 1:   //001
    case 3:   //011
      Motor_Move(SPEED_LV1, SPEED_LV1, -SPEED_LV1, -SPEED_LV1);  //Turn Left
      break;
    default:
      break;
  }
  }
  delay(1500);
}

void turnLeft(){
        // Make a left turn
      Motor_Move(-SPEED_LV1, -SPEED_LV1, -SPEED_LV1, -SPEED_LV1);

      delay(150);

      Motor_Move(SPEED_LV1 + 65, SPEED_LV1 + 65, -SPEED_LV1 - 40, -SPEED_LV1 - 40);

      delay(500);

      Motor_Move(0, 0, 0, 0);

      Track_Read();
      while (sensorValue[3] == 0){
        Motor_Move(SPEED_LV1 + 65, SPEED_LV1 + 65, -SPEED_LV1 - 40, -SPEED_LV1 - 40);
      }
}

void turnRight(){
        // Make a right turn
      Motor_Move(-SPEED_LV1, -SPEED_LV1, -SPEED_LV1, -SPEED_LV1);

      delay(150);

      Motor_Move(-SPEED_LV1 - 30, -SPEED_LV1 - 30, SPEED_LV1 + 50, SPEED_LV1 + 50);

      delay(500);

      Motor_Move(0, 0, 0, 0);

      Track_Read();
      while (sensorValue[3] == 0){
        Motor_Move(-SPEED_LV1 - 30, -SPEED_LV1 - 30, SPEED_LV1 + 50, SPEED_LV1 + 50);
      }
}

void setup()
{

  Serial.begin(115200); // You can adjust the baud rate (9600 is a common value)
  delay(1000);


    WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  config.database_url = "embedded-project-71b16-default-rtdb.europe-west1.firebasedatabase.app";
  config.signer.tokens.legacy_token = "9AEvt5kBUUDJFtTO4HmcRioDRvuD6Ytb6YhmZ0F9";
  Firebase.reconnectNetwork(true);
  Firebase.begin(&config, &auth);  // Initialize Firebase


  Track_Setup();   //Trace module initialization
  PCA9685_Setup(); //Motor drive initialization
  Emotion_Setup();

  fetchBallCoordinates();  // Fetch ball coordinates from Firebase
  calculations();
  /*followLineX();
  followLineNegX();
  followLineY();
  followLineNegY();
  turnLeft();
  turnRight();
  adjust();
*/

}

void loop() {
  // Your main control logic goes here
  // Move to the closest ball to the starting position

  if (finish){
    return;
  }

  Track_Read();

  while (sensorValue[3] == 5){
  Track_Read();
  if (sensorValue[3] == 5)
  Motor_Move(-SPEED_LV1, -SPEED_LV1, -SPEED_LV1, -SPEED_LV1); //Move Forward
  }

  while (myX != goFirstX){
      adjust();
      followLineX();
  }

  //recoil();
  turnLeft();

  while (myY != goFirstY){
    adjust();
    followLineY();
  }

  delay(2000); //first ball collected

  while (myY != 3){
    adjust();
    followLineY();
  }

  //recoil();
  turnRight();

  while (myX !=3){
    adjust();
    followLineX();
  }

  delay (2000); // ball 1 in basket

  //recoil();
  turnRight();

  while (myY != goSecondY){
    adjust();
    followLineNegY();
  }

  //recoil();
  turnRight();

  while (myX != goSecondX){
    adjust();
    followLineNegX();
  }

  delay(2000); // Second ball grab

  //recoil();
  turnRight();

  while (myY != 3){
    adjust();
    followLineY();
  }

  //recoil();
  turnRight();

  while (myX != 3){
    adjust();
    followLineX();
  }

  delay(2000); // Second ball in basket

  //recoil();
  turnRight();

  while (myY != goThirdY){
    adjust();
    followLineNegY();
  }

  //recoil();
  turnRight();

  while (myX != goThirdX){
    adjust();
    followLineNegX();
  }

  delay(2000); // Third ball grab

  //recoil();
  turnRight();

  while (myY != 3){
    adjust();
    followLineY();
  }

  //recoil();
  turnRight();

  while (myX != 3){
    adjust();
    followLineX();
  }

  delay(500); // Third ball in basket

  finish = true;

}

