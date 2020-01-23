#include <SPI.h>
#include <SD.h>

// right sonar
#define trigRight A0
#define echoRight A1

// middle sonar
#define trigMid A2
#define echoMid A3

// left sonar
#define trigLeft A4
#define echoLeft A5

#define MotorLeft_IN1 3
#define MotorLeft_IN2 4
#define MotorRight_IN3 5
#define MotorRight_IN4 6

#define MotorLeft_PWM 9
#define MotorRight_PWM 10

//#define MISO 12
//#define MOSI 11
//#define SCK 13
#define chipSelect 7


float distanceLeft = 0.00;
float distanceMid = 0.00;
float distanceRight = 0.00;
float carSpeed = 0.00;

void setup(){
  Initialize_SDcard();
  // motor pins setup
  pinMode(MotorLeft_IN1, OUTPUT);
  pinMode(MotorLeft_IN2, OUTPUT);
  pinMode(MotorRight_IN3, OUTPUT);
  pinMode(MotorRight_IN4, OUTPUT);
  pinMode(MotorLeft_PWM, OUTPUT);
  pinMode(MotorRight_PWM, OUTPUT);

  // sonars' pins setup
  pinMode(trigLeft, OUTPUT);
  pinMode(echoLeft, INPUT);
  
  pinMode(trigMid, OUTPUT);
  pinMode(echoMid, INPUT);
  
  pinMode(trigRight, OUTPUT);
  pinMode(echoRight, INPUT);
}

void loop(){ 
  distanceLeft = leftSonar();
  distanceMid = midSonar();
  distanceRight = rightSonar();
  
  if (distanceMid < 40 && distanceRight < 40 && distanceLeft < 40) // obstacle infront of all 3 sides
  {
    Write_SDcard();
    carStop();
    delay(10);
    carSpeed = 100;
    analogWrite(MotorLeft_PWM, carSpeed);
    analogWrite(MotorRight_PWM, carSpeed);
    while (distanceRight < 60 || distanceLeft < 60){
      carBackWard();
      distanceLeft = leftSonar();
      distanceRight = rightSonar();
      Write_SDcard();
    }
    carStop();
    delay(10);
    if(distanceRight > distanceLeft){
      carRight();
    }else{
      carLeft();
    }
  } else if (distanceMid < 40 && distanceRight < 40 && distanceLeft > 40) // obstacle on right and front sides
  {
    carStop();
    delay(10);
    carSpeed = 120;
    analogWrite(MotorLeft_PWM, carSpeed);
    analogWrite(MotorRight_PWM, carSpeed);
    carLeft();
    Write_SDcard();
  } else if (distanceMid < 40 && distanceRight > 40 && distanceLeft < 40) // obstacle on left and front sides
  {
    carSpeed = 120;
    analogWrite(MotorLeft_PWM, carSpeed);
    analogWrite(MotorRight_PWM, carSpeed);
    carStop();
    delay(10);
    carRight();
    Write_SDcard();
  } else if (distanceMid < 40 && distanceRight > 40 && distanceLeft > 40) // obstacle on front sides
  {
    carStop();
    delay(10);
    carSpeed = 180;
    analogWrite(MotorLeft_PWM, carSpeed);
    analogWrite(MotorRight_PWM, carSpeed);
    if(distanceRight>distanceLeft){
      carRight();
    }else{
      carLeft();
    }
    Write_SDcard();
  } else if (distanceMid > 40 && distanceRight > 40 && distanceLeft < 40) // obstacle on left sides
  {
    carStop();
    delay(10);
    carSpeed = 180;
    analogWrite(MotorLeft_PWM, carSpeed);
    analogWrite(MotorRight_PWM, carSpeed);
    carRight();
    delay(50);
    carForward();
    Write_SDcard();
   } else if (distanceMid > 40 && distanceRight < 40 && distanceLeft > 40) // obstacle on right sides
   {
    carStop();
    delay(10);
    carSpeed = 180;
    analogWrite(MotorLeft_PWM, carSpeed);
    analogWrite(MotorRight_PWM, carSpeed);
    carLeft();
    delay(50);
    carForward();
    Write_SDcard();
    } else {
      carSpeed = 255;
      analogWrite(MotorLeft_PWM, carSpeed);
      analogWrite(MotorRight_PWM, carSpeed);
      carForward();
      Write_SDcard();
    }
}

void carForward(){
  digitalWrite(MotorLeft_IN1, HIGH);
  digitalWrite(MotorLeft_IN2, LOW);
  digitalWrite(MotorRight_IN3, HIGH);
  digitalWrite(MotorRight_IN4, LOW);
}

void carBackWard(){
  digitalWrite(MotorLeft_IN1, LOW);
  digitalWrite(MotorLeft_IN2, HIGH);
  digitalWrite(MotorRight_IN3, LOW);
  digitalWrite(MotorRight_IN4, HIGH);
}

void carRight(){
  digitalWrite(MotorLeft_IN1, HIGH);
  digitalWrite(MotorLeft_IN2, LOW);
  digitalWrite(MotorRight_IN3, LOW);
  digitalWrite(MotorRight_IN4, HIGH);
}

void carLeft(){
  digitalWrite(MotorLeft_IN1, LOW);
  digitalWrite(MotorLeft_IN2, HIGH);
  digitalWrite(MotorRight_IN3, HIGH);
  digitalWrite(MotorRight_IN4, LOW);
}

void carStop(){
  digitalWrite(MotorLeft_IN1, LOW);
  digitalWrite(MotorLeft_IN2, LOW);
  digitalWrite(MotorRight_IN3, LOW);
  digitalWrite(MotorRight_IN4, LOW);
}

int leftSonar(void){
  float duration = 0.00;

  digitalWrite(trigLeft, LOW);
  delayMicroseconds(2);

  digitalWrite(trigLeft, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigLeft, LOW);

  duration = pulseIn(echoLeft, HIGH);
  return (duration/58.82);
}

int midSonar(void){
  float duration = 0.00;

  digitalWrite(trigMid, LOW);
  delayMicroseconds(2);

  digitalWrite(trigMid, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigMid, LOW);

  duration = pulseIn(echoMid, HIGH);
  return (duration/58.82);
}

int rightSonar(void){
  float duration = 0.00;

  digitalWrite(trigRight, LOW);
  delayMicroseconds(2);

  digitalWrite(trigRight, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigRight, LOW);

  duration = pulseIn(echoRight, HIGH);
  return (duration/58.82);
}

void Write_SDcard(){
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("dataLog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.print(distanceLeft); //Store data
    dataFile.print(","); //Move to next column using a ","

    dataFile.print(distanceMid);
    dataFile.print(",");

    dataFile.print(distanceRight);
    dataFile.print(",");

    dataFile.print(carSpeed);
    dataFile.print(",");

    dataFile.println(); //End of Row move to next row
    dataFile.close(); //Close the file
  }
  else
  Serial.println("SD card writing failed");
}

void Initialize_SDcard(){
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("dataLog.txt", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println("distanceLeft, distanceMid, distanceRight, carSpeed"); //Write the first row of the excel file
    dataFile.close();
  }
}
