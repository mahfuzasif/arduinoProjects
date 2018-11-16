#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int leftButton = 22;
const int rightButton = 23;
const int leftIR = 24;
const int rightIR = 25;
const int leftLed =  26;
const int rightLed =  27;
const int trigLeft = 31;
const int echoLeft = 30;
const int trigRight = 33;
const int echoRight = 32;
const int safetyLeft = 34;
const int safetyRight = 35;

int contrast = 85;     //not needed if pot is connected to pin 6 i.e. Vo of lcd
int buttonState = 0;
int irState = HIGH;
long duration, distance, sonarLeft, sonarRight;

void setup() {
  Serial.begin(19200);
  Serial.println("System started");
  analogWrite(6, contrast);     //not needed if pot is connected to pin 6 i.e. Vo of lcd
  lcd.begin(16, 2);
  lcd.print("System started");
  pinMode(leftButton, INPUT);
  pinMode(rightButton, INPUT);
  pinMode(leftIR, INPUT);
  pinMode(rightIR, INPUT);
  pinMode(leftLed, OUTPUT);
  pinMode(rightLed, OUTPUT);
  pinMode(trigLeft, OUTPUT);
  pinMode(echoLeft, INPUT);
  pinMode(safetyLeft, OUTPUT);

  pinMode(trigRight, OUTPUT);
  pinMode(echoRight, INPUT);
  pinMode(safetyRight, OUTPUT);
}

void loop() {
  SonarSensor(trigLeft, echoLeft);
  sonarLeft = distance;
  SonarSensor(trigRight, echoRight);
  sonarRight = distance;

  if (sonarLeft <= 40) { // if distance is less than 40cm~16inch turn the LED ON
    digitalWrite(safetyLeft, HIGH);
    Serial.print("Unsafe Left: ");
    Serial.print(sonarLeft);
    Serial.println(" cm");
  } else {
    Serial.print("Left: ");
    Serial.print(sonarLeft);
    Serial.println(" cm");
    digitalWrite(safetyLeft, LOW);
  }

  if (sonarRight <= 40) {
    digitalWrite(safetyRight, HIGH);
    Serial.print("Unsafe Right: ");
    Serial.print(sonarRight);
    Serial.println(" cm");
  } else {
    Serial.print("Right: ");
    Serial.print(sonarRight);
    Serial.println(" cm");
    digitalWrite(safetyRight, LOW);
  }

  lcd.setCursor(0, 0);
  if (digitalRead(leftButton) == HIGH || digitalRead(leftIR) == LOW) {
    lcd.clear();
    if (sonarLeft > 40) {
      blinkLeft();
    }
  } else if (digitalRead(rightButton) == HIGH || digitalRead(rightIR) == LOW) {
    lcd.clear();
    if (sonarRight > 40) {
      blinkRight();
    }
  }
}

void blinkLeft() {
  Serial.print("Blinking left");
  lcd.print("Blinking left");
  lcd.autoscroll();
  for (int i = 0; i < 5; i++) {
    digitalWrite(leftLed, HIGH);
    Serial.print(".");
    lcd.print(".");
    delay(700);
    digitalWrite(leftLed, LOW);
    delay(600);
  }
  lcd.noAutoscroll();
  lcd.clear();
  Serial.println("\nLed off");
  lcd.print("Led off");
}

void blinkRight() {
  Serial.print("Blinking right");
  lcd.print("Blinking right");
  lcd.autoscroll();
  for (int i = 0; i < 5; i++) {
    digitalWrite(rightLed, HIGH);
    Serial.print(".");
    lcd.print(".");
    delay(700);
    digitalWrite(rightLed, LOW);
    delay(600);
  }
  lcd.noAutoscroll();
  lcd.clear();
  Serial.println("\nLed off");
  lcd.print("Led off");
}

void SonarSensor(int trigPinSensor, int echoPinSensor) {
  digitalWrite(trigPinSensor, LOW);// put trigpin LOW
  delayMicroseconds(2);// wait 2 microseconds
  digitalWrite(trigPinSensor, HIGH);// switch trigpin HIGH
  delayMicroseconds(10); // wait 10 microseconds
  digitalWrite(trigPinSensor, LOW);// turn it LOW again

  //read the distance
  duration = pulseIn(echoPinSensor, HIGH);
  distance = (duration / 2) / 29.1; // getting distance in cm
}

