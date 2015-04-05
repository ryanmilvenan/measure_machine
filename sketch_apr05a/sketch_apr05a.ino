#include <FiniteStateMachine.h>
#include <Bounce2.h>
#include <LiquidCrystal.h>

//I/O
LiquidCrystal lcd(13,12,11,10,9,8);
int pumpControlPin = 7;
//int scaleVoltagePin = A0;

//Control Buttons
int backButtonPin = 0;
Bounce backButton = Bounce();

int forwardButtonPin = 1;
Bounce forwardButton = Bounce();

int acknowledgeButtonPin = 2;
Bounce acknowledgeButton = Bounce();

int activateButtonPin = 3;
Bounce activateButton = Bounce();

int cancelButtonPin = 4;
Bounce cancelButton = Bounce();

int dbInterval = 10;

State Nav = State(empty, navUpdate, cleanUp);
State Liquid = State(liquidUpdate);
State Solid = State(solidUpdate);

FSM measureMachine = FSM(Nav);

bool back = false;
bool forward = false;
bool acknowledged = false;
String dispense;
String measurement;
double amount = 0.0;

void setup() {
  Serial.begin(9600);
  //I/O Initialization
  pinMode(pumpControlPin, OUTPUT);
  //pinMode(scaleVoltagePin, INPUT);
  
  //Button Initialization
  pinMode(backButtonPin, INPUT_PULLUP);
  pinMode(forwardButtonPin, INPUT_PULLUP);
  pinMode(acknowledgeButtonPin, INPUT_PULLUP);
  pinMode(activateButtonPin, INPUT_PULLUP);
  pinMode(cancelButtonPin, INPUT_PULLUP);
  
  backButton.attach(backButtonPin);
  forwardButton.attach(forwardButtonPin);
  acknowledgeButton.attach(acknowledgeButtonPin);
  activateButton.attach(activateButtonPin);
  cancelButton.attach(cancelButtonPin);
  
  backButton.interval(dbInterval);
  forwardButton.interval(dbInterval);
  acknowledgeButton.interval(dbInterval);
  activateButton.interval(dbInterval);
  cancelButton.interval(dbInterval);

  //LCD Initialization 
  lcd.begin(16, 2);
  lcd.clear();
  

}

void loop() {
  // put your main code here, to run repeatedly:
  measureMachine.update();
}

//Utility Functions
void navUpdate() {
  if(back) {
    measureMachine.transitionTo(Liquid);
  }
  if(forward) {
    measureMachine.transitionTo(Solid);
  }
  lcd.setCursor(0,0);
  lcd.print(" measureMachine");
  lcd.setCursor(0,1);
  lcd.print("<-Liquid Solid->");
  checkBackNav();
  checkForwardNav();
}

void checkBackNav() {
  int backValue = backButton.read();
  if(backValue == LOW) {
    Serial.println("Got Called");
    back = true;
  }
    
}

void checkForwardNav() {
  int forwardValue = forwardButton.read();
  if(forwardValue == LOW)
    forward = true;
}

void empty() {
}

void cleanUp() {
  back = false;
  forward = false;
  acknowledged = false;
}

void reset() {
  back = false;
  forward = false;
  acknowledged = false;
  dispense = "";
  measurement = "";
  amount = 0.0;
}

void liquidUpdate() {
  lcd.setCursor(0,0);
  lcd.print("liquid");
}

void solidUpdate() {
  lcd.setCursor(0,0);
  lcd.print("solid");
}
