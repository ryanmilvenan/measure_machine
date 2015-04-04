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

//State
int mode = 0;
int dispenseType = 0;
int measurementType = 0;
bool dispenseAcknowledged = false;
bool measureAcknowledged = false;
bool activated = false;

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
  updateButtonStates();
  
  
  float voltage;
  switch(mode) {
      //Navigation mode
      case 0:
        Serial.println(mode);
        lcd.setCursor(0,0);
        lcd.print(" measureMachine");
        lcd.setCursor(0,1);
        lcd.print("<-Liquid Solid->");
        checkBackNav();
        checkForwardNav();
        checkCancel();
        checkActivation();
      break;
      
      //Liquid Mode
      case 1:
        Serial.println(mode);

        displayLiquidDispenseType();
        displayLiquidMeasurementType();
        checkCancel();
        checkActivation();
      break;
      
      //Solid Mode
      case 2:
        displaySolidDispenseType();
        displaySolidMeasurementType();
        checkCancel();
        checkActivation();
        //value = analogRead(scaleVoltagePin);
        //voltage = value  * (5.0 / 1023.0) * 10000000000;
      break;
  }
   
}


//Utility Functions
void updateButtonStates() {
  backButton.update();
  forwardButton.update();
  acknowledgeButton.update();
  activateButton.update();
  cancelButton.update();
}

void checkCancel() {
  int cancelValue = cancelButton.read();
  if(cancelValue == LOW) {
    resetState();
  }
}

void resetState() {
  activated = false;
  mode = 0;
  dispenseType = 0;
  measurementType = 0;
  dispenseAcknowledged = false;
  measureAcknowledged = false;
}

void checkActivation() {
  int activateValue = activateButton.read();
  if(activateValue == LOW && !activated) {
    activated = true;
  }
}

void checkBackNav() {
  int backValue = backButton.read();
  if(backValue == LOW)
    mode = 1;
}

void checkForwardNav() {
  int forwardValue = forwardButton.read();
  if(forwardValue == LOW)
    mode = 2;
}

void checkBackDispense() {
  int backValue = backButton.read();
  if(backValue == LOW) {
    dispenseType = 0;
  }
}

void checkForwardDispense() {
  int forwardValue = forwardButton.read();
  if(forwardValue == LOW) {
    dispenseType = 1;
  }
}

void checkBackMeasurement() {
  int backValue = backButton.read();
  if(backValue == LOW) {
    measurementType = 0;
  }
}

void checkForwardMeasurement() {
  int forwardValue = forwardButton.read();
  if(forwardValue == LOW) {
    measurementType = 1;
  }
}

void displayLiquidDispenseType() {
  switch(dispenseType) {
    //Water
    case 0:
    lcd.setCursor(0,0);
    lcd.print("<-    Water   ->");
    checkBackDispense();
    checkForwardDispense();
    checkDispenseAcknowledge();
    break;
    
    //Olive Oil
    case 1:
    lcd.setCursor(0,0);
    lcd.print("<-  Olive Oil ->");
    checkBackDispense();
    checkForwardDispense();
    checkDispenseAcknowledge();
    break;
  }
}

void displayLiquidMeasurementType() {
  if(!dispenseAcknowledged) {
    lcd.setCursor(0,1);
    lcd.print("                ");
  } else if(dispenseAcknowledged && !measureAcknowledged) {
    lcd.setCursor(0,1);
    switch(measurementType) {
      //Cups
      case 0:
        lcd.print("<-    Cups    ->");
        checkBackMeasurement();
        checkForwardMeasurement();
        //checkMeasureAcknowledge();
        beginDispensingLiquidCups(10);
      break;
      
      //Tablespoons
      case 1:
        lcd.print("<-   Tblsp    ->");
        checkBackMeasurement();
        checkForwardMeasurement();
        //checkMeasureAcknowledge();
        beginDispensingLiquidSpoons(10);
      break;
    }
  } else if(dispenseAcknowledged && measureAcknowledged) {
    lcd.setCursor(0,0);
    lcd.print("Rdy to dispense ");
    lcd.setCursor(0,1);
    lcd.print(" Green to begin ");
  }
}

void displaySolidDispenseType() {

}

void displaySolidMeasurementType() {

}

void checkDispenseAcknowledge() {
  int acknowledgeValue = acknowledgeButton.read();
  if(acknowledgeValue == LOW && !dispenseAcknowledged)
    dispenseAcknowledged = true;
}

void checkMeasureAcknowledge() {
  int acknowledgeValue = acknowledgeButton.read();
  if(acknowledgeValue == LOW && !measureAcknowledged)
    measureAcknowledged = true;
}

void clearLine() {
  lcd.print("                ");
  lcd.setCursor(0,1);
}

//Dispense Functions
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
