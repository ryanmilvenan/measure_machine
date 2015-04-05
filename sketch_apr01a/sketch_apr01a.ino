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

int dbInterval = 20;

//State
int mode = 0;
int dispenseType = 0;
int measurementType = 0;
double amount = 0.0;
bool dispenseAcknowledged = false;
bool measureAcknowledged = false;
bool amountAcknowledged = false;
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
        lcd.setCursor(0,0);
        lcd.print(" measureMachine");
        lcd.setCursor(0,1);
        lcd.print("<-Liquid Solid->");
        checkBackNav();
        checkForwardNav();
      break;
      
      //Liquid Mode
      case 1:
        //displayLiquidDispenseType();
        //displayLiquidMeasurementType();
        checkAmount();
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
  amountAcknowledged = false;
  amount = 0.0;
}



void displayLiquidDispenseType() {
  switch(dispenseType) {
    //Water
    case 0:
    lcd.setCursor(0,0);
    lcd.print("<-    Water   ->");
    if(!dispenseAcknowledged) {
      lcd.setCursor(0,1);
      lcd.print("                ");
      checkBackDispense();
      checkForwardDispense();
      checkDispenseAcknowledge();
    } else {
      displayLiquidMeasurementType();
    }
    break;
    
    //Olive Oil
    case 1:
    lcd.setCursor(0,0);
    lcd.print("<-  Olive Oil ->");
    if(!dispenseAcknowledged) {
      lcd.setCursor(0,1);
      lcd.print("                ");
      checkBackDispense();
      checkForwardDispense();
      checkDispenseAcknowledge();
    } else {
      displayLiquidMeasurementType();
    }
    break;
  }
}

void displayLiquidMeasurementType() {
  lcd.setCursor(0,1);
  switch(measurementType) {
    //Cups
    case 0:
      lcd.print("<-    Cups    ->");
      if(!measureAcknowledged) {
        checkBackMeasurement();
        checkForwardMeasurement();
        checkMeasureAcknowledge();
      } else {
        checkAmount();
      }
    break;
    
    //Tablespoons
    case 1:
      lcd.print("<-   Tblsp    ->");
      if(!measureAcknowledged) {
        checkBackMeasurement();
        checkForwardMeasurement();
        checkMeasureAcknowledge();
      } else {
        checkAmount();
      }
    break;
  }
}

void displaySolidDispenseType() {

}

void displaySolidMeasurementType() {

}

void checkAmount() {
  //if(dispenseAcknowledged && measureAcknowledged) {
    lcd.setCursor(0,0);
    lcd.print("How much?         ");
    lcd.setCursor(0,1);
    char temp[6];
    String amountString;
    dtostrf(amount,1,2, temp);
    amountString = String(temp);
    amountString += "          ";
    lcd.print(amountString);
    switch(measurementType) {
      //Cups
      case 0:
        checkBackAmountCups();
        checkForwardAmountCups();
        checkAmountAcknowledge();
        beginDispensingLiquidCups(amount);
      break;
      
      //Tablespoons
      case 1:
        checkBackAmountTbls();
        checkForwardAmountTbls();
        lcd.print(amountString);
        checkAmountAcknowledge();
        beginDispensingLiquidSpoons(amount);
      break;
    }
  //}
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


//Button
void checkActivation() {
  int activateValue = activateButton.read();
  if(activateValue == LOW && !activated && dispenseAcknowledged && measureAcknowledged && amountAcknowledged) {
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

void checkBackAmountCups() {
  int backValue = backButton.read();
  if(backValue == LOW && amount > 0.0) {
    amount -= 0.25;
  }
}

void checkForwardAmountCups() {
  int forwardValue = forwardButton.read();
  if(forwardValue == LOW && amount < 2.0) {
    amount += 0.25;
  }
}

void checkBackAmountTbls() {
  int backValue = backButton.read();
  if(backValue == LOW && amount > 0.0) {
    amount -= 0.25;
  }
}

void checkForwardAmountTbls() {
  int forwardValue = forwardButton.read();
  if(forwardValue == LOW && amount < 2.0) {
    amount += 0.25;
  }
}

void checkDispenseAcknowledge() {
  int acknowledgeValue = acknowledgeButton.read();
  if(acknowledgeValue == LOW && !dispenseAcknowledged)
    dispenseAcknowledged = true;
}

void checkMeasureAcknowledge() {
  int acknowledgeValue = acknowledgeButton.read();
  if(acknowledgeValue == LOW && dispenseAcknowledged && !measureAcknowledged)
    measureAcknowledged = true;
}

void checkAmountAcknowledge() {
  int acknowledgeValue = acknowledgeButton.read();
  if(acknowledgeValue == LOW && !amountAcknowledged) {
    amountAcknowledged == true;
  }
}
