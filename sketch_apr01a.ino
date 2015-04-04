#include <LiquidCrystal.h>

//I/O
LiquidCrystal lcd(12,11,5,4,3,2);
int pumpControlPin = 7;
int scaleVoltagePin = A0;

//Control Buttons
int backButtonPin;
int forwardButtonPin;
int activateButtonPin;
int cancelButtonPin;

//State
int mode = 0;
bool activated = false;

void setup() {
  pinMode(pumpControlPin, OUTPUT);
  pinMode(scaleVoltagePin, INPUT);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Hey Erin");
}

void loop() {
  int value;
  float voltage;
  switch(mode) {
      //Navigation mode
      
      case 0:
        lcd.setCursor(0,1);
        value = analogRead(scaleVoltagePin);
        voltage = value  * (5.0 / 1023.0) * 10000000000;
        lcd.print(voltage);
      break;
      
      //Solid Mode
      case 1:
        //beginDispensingSolidSpoons();
      break;
      
      //Liquid Mode
      case 2:
      break;
  }
 
  // put your main code here, to run repeatedly:
  
}

void beginDispensingSolidSpoons(double amount) {
  if(activated) {
  }
}

void beginDispensingLiquidSpoons(double amount) {
  if(activated) {
  }
}

void beginDispensingLiquidCups(double amount) {
  if(activated) {
    digitalWrite(pumpControlPin, HIGH);
  }
}
