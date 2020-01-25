#include <LiquidCrystal.h> //importing builtIn library for LCD

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);      //setting pins for LCD
int tempPin = A0;     //the output pin of (LM35) temperature sensor
int buzzer = 10;      //buzzer pin
int led = 8;          //led pin
float temp;
float tempMax = 47.0;     //maximum temperature when buzzer will start

void setup() {
//  Serial.begin(9600);     //start Serial Monitor
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(tempPin, INPUT);
  lcd.begin(16, 2);
}

void loop() {
  temp = readTemp();      //get the temperature

//  Serial.println(temp);     //showing temperature in Serial Monitor of Arduino
  if (temp > tempMax) {
    lcd.print("Alert!! ");      //Displaying Alert message for high temperature
    lcd.print(temp);            //Displaying temperature
    lcd.print("C");
    tone(buzzer, 1500, 500);      //buzzer on with 1500Hz for 500milliseconds
    digitalWrite(led, HIGH);      //turn on LED
  } else {
    digitalWrite(led, LOW);     //else turn LED off
    lcd.print("TEMP: ");
    lcd.print(temp);      //Displaying temperature
    lcd.print("C ");
  }
  delay(1000);
  lcd.clear();
}

float readTemp() {     //get the temperature and convert it to celsius
  temp = analogRead(tempPin);
  return (temp * 0.48828125);
}
