#include <Button.h>
#include <FiniteStateMachine.h>
#include <LiquidCrystal.h>

const byte NUM_LIQUIDS = 2;
const byte NUM_SOLIDS = 2;
const byte NUM_MEASUREMENTS = 2;

//I/O
LiquidCrystal lcd(7,6,5,4,3,2);
int pumpControlPin = 26;
int scaleVoltagePin = A0;
int stimulatorPin = 8;

//Control Buttons
int backButtonPin = 10;
Button backButton = Button(backButtonPin, PULLUP);

int forwardButtonPin = 9;
Button forwardButton = Button(forwardButtonPin, PULLUP);

int acknowledgeButtonPin = 13;
Button ackButton = Button(acknowledgeButtonPin, PULLUP);

int activateButtonPin = 12;
Button activateButton = Button(activateButtonPin, PULLUP);

int cancelButtonPin = 11;
Button cancelButton = Button(cancelButtonPin, PULLUP);

//Finite State Machine 
State Nav = State(empty, navUpdate, cleanDisplay);

State LiquidDispenseSelect = State(cleanBottomLine, liquidDispenseSelectUpdate, empty);
State LiquidMeasurementSelect = State(empty, liquidMeasurementSelectUpdate, cleanDisplay);
State LiquidAmountSelect = State(empty, liquidAmountSelectUpdate, cleanDisplay);
State LiquidStart = State(empty, liquidStartUpdate, empty);

State SolidDispenseSelect = State(cleanBottomLine, solidDispenseSelectUpdate, empty);
State SolidMeasurementSelect = State(empty, solidMeasurementSelectUpdate, cleanDisplay);
State SolidAmountSelect = State(empty, solidAmountSelectUpdate, cleanDisplay);
State SolidStart = State(empty, solidStartUpdate, empty);

FSM measureMachine = FSM(Nav);

char liquids[2][18] = {"<-    Water   ->",
                       "<-  OliveOil  ->"};

char solids[2][18] = {"<-   Parsley  ->",
                      "<-  Cinnamon  ->"};
                      
char measurements[2][18] = {"<-    Cups    ->",
                            "<-    Tbls    ->"};
                            
char tbls[6][18] = {"    1/8 tsp   ->",
                    "<-  1/4 tsp   ->",
                    "<-  1/2 tsp   ->",
                    "<-  1 tsp     ->",
                    "<-  1/2 tbls  ->",
                    "<-  1 tbls    ->"};

double tblsEquiv[6] = {0.41,
                          0.83,
                          0.16,
                          0.33,
                          0.5,
                          1};
                          
int liquidIdx = 0;
int solidIdx = 0;
int liqMeasureIdx = 0;
int tblsIdx = 0;
int liqStartTime;
double liqEndTime;
int solStartTime;
double solEndTime;
double liqAmount = 0.0;
double solAmount = 0.0;
bool liquidActive = false;
bool solidActive = false;
bool pumping = false;
bool stimulating = false;


void setup() {
  Serial.begin(9600);
  pinMode(pumpControlPin, OUTPUT);
  pinMode(scaleVoltagePin, INPUT);
  pinMode(stimulatorPin, OUTPUT);

  //LCD Initialization 
  lcd.begin(16, 2);
  lcd.clear();
}

void loop() {
  dispenseLiquid();
  dispenseSolid();
  measureMachine.update();
}

//Utility Functions
void navUpdate() {
  if(backButton.uniquePress()) {
    measureMachine.transitionTo(SolidDispenseSelect);
  }
  if(forwardButton.uniquePress()) {
    measureMachine.transitionTo(LiquidDispenseSelect);
  }
  lcd.setCursor(0,0);
  lcd.print(" measureMachine");
  lcd.setCursor(0,1);
  lcd.print("<-Solid Liquid->");
}

void empty() {
}

void reset() {
  liqAmount = 0.0;
  solAmount = 0.0;
  liquidIdx = 0;
  solidIdx = 0;
  liqMeasureIdx = 0;
  cleanDisplay();
}

/***********************************
*              LIQUID          
***********************************/

/////////LIQUID DISPENSE SELECT
void liquidDispenseSelectUpdate() {
  if(cancelButton.uniquePress()) {
    reset();
    measureMachine.transitionTo(Nav);
  }
  
  if(backButton.uniquePress()) {
    liquidIdx = --liquidIdx % NUM_LIQUIDS;
  }
  
  if(forwardButton.uniquePress()) {
    liquidIdx = ++liquidIdx % NUM_LIQUIDS;
  }
  
  if(ackButton.uniquePress()) {
    measureMachine.transitionTo(LiquidMeasurementSelect);
  }
 
  lcd.setCursor(0,0);
  lcd.print(liquids[liquidIdx]);
}


//////////LIQUID MEASUREMENT SELECT
void liquidMeasurementSelectUpdate() {
  if(cancelButton.uniquePress()) {
    reset();
    measureMachine.transitionTo(Nav);
  }
  
  if(backButton.uniquePress()) {
    liqMeasureIdx = --liqMeasureIdx % NUM_MEASUREMENTS;
  }
  
  if(forwardButton.uniquePress()) {
    liqMeasureIdx = ++liqMeasureIdx % NUM_MEASUREMENTS;
  }
  
  if(ackButton.uniquePress()) {
    measureMachine.transitionTo(LiquidAmountSelect);
  }
  
  lcd.setCursor(0,1);
  lcd.print(measurements[liqMeasureIdx]);
}

//////////AMOUNT SELECT
void liquidAmountSelectUpdate() {
  if(cancelButton.uniquePress()) {
    reset();
    measureMachine.transitionTo(Nav);
  }
  
  if(ackButton.uniquePress()) {
    measureMachine.transitionTo(LiquidStart);
  }
  
  lcd.setCursor(0,0);
  lcd.print("How much?");
  
  switch(liqMeasureIdx) {
    //Cups
    case 0:
      if(backButton.uniquePress()) {
        if(liqAmount > 0) {
          liqAmount -= 0.25;
        }
      }
      
      if(forwardButton.uniquePress()) {
        if(liqAmount < 2) {
          liqAmount += 0.25;
        }
      }
      
      lcd.setCursor(0,1);
      lcd.print(liqAmount);
    break;
    
    //Spoons
    case 1:
      if(backButton.uniquePress()) {
        if(liqAmount > 0) {
          liqAmount -= 1;
        }
      }
      
      if(forwardButton.uniquePress()) {
        if(liqAmount < 6) {
          liqAmount += 1;
        }
      }
      lcd.setCursor(0,1);
      lcd.print(tbls[(int)liqAmount]);
    break;
  }
}

//////////AMOUNT SELECT
void liquidStartUpdate() {
  if(cancelButton.uniquePress()) {
    reset();
    measureMachine.transitionTo(Nav);
  }
  
  if(activateButton.uniquePress()) {
    liquidActive = true;
    measureMachine.transitionTo(Nav);
  }
  
  lcd.setCursor(0,0);
  lcd.print("Press Green Btn ");
  lcd.setCursor(0,1);
  lcd.print("to begin"); 
}

/***********************************
*              SOLID          
***********************************/

/////////SOLID DISPENSE SELECT
void solidDispenseSelectUpdate() {
  if(cancelButton.uniquePress()) {
    reset();
    measureMachine.transitionTo(Nav);
  }
  
  if(backButton.uniquePress()) {
    solidIdx = --solidIdx % NUM_SOLIDS;
  }
  
  if(forwardButton.uniquePress()) {
    solidIdx = ++solidIdx % NUM_SOLIDS;
  }
  
  if(ackButton.uniquePress()) {
    measureMachine.transitionTo(SolidMeasurementSelect);
  }
 
  lcd.setCursor(0,0);
  lcd.print(solids[solidIdx]);
}


//////////SOLID MEASUREMENT SELECT
void solidMeasurementSelectUpdate() {
  if(cancelButton.uniquePress()) {
    reset();
    measureMachine.transitionTo(Nav);
  }
  
  if(backButton.uniquePress()) {
    //measureIdx = --measureIdx % NUM_MEASUREMENTS;
  }
  
  if(forwardButton.uniquePress()) {
    //measureIdx = ++measureIdx % NUM_MEASUREMENTS;
  }
  
  if(ackButton.uniquePress()) {
    measureMachine.transitionTo(SolidAmountSelect);
  }
  
  lcd.setCursor(0,1);
  lcd.print(measurements[1]);
}

//////////AMOUNT SELECT
void solidAmountSelectUpdate() {
  if(cancelButton.uniquePress()) {
    reset();
    measureMachine.transitionTo(Nav);
  }
  
  if(ackButton.uniquePress()) {
    measureMachine.transitionTo(SolidStart);
  }
  
  lcd.setCursor(0,0);
  lcd.print("How much?");
  
  if(backButton.uniquePress()) {
    if(solAmount > 0) {
      solAmount -= 1;
    }
  }
      
  if(forwardButton.uniquePress()) {
    if(solAmount < 6) {
      solAmount += 1;
    }
  }
  
  lcd.setCursor(0,1);
  lcd.print(tbls[(int)solAmount]);
  
}

//////////AMOUNT SELECT
void solidStartUpdate() {
  if(cancelButton.uniquePress()) {
    reset();
    measureMachine.transitionTo(Nav);
  }
  
  if(activateButton.uniquePress()) {
    solidActive = true;
    measureMachine.transitionTo(Nav);
  }
  
  lcd.setCursor(0,0);
  lcd.print("Press Green Btn ");
  lcd.setCursor(0,1);
  lcd.print("to begin"); 
}


/***********************************
*              UTILITY          
***********************************/
void cleanBottomLine() {
  lcd.setCursor(0,1);
  lcd.print("               ");
}

void cleanDisplay() {
  lcd.clear();
}

void solidUpdate() {
  lcd.setCursor(0,0);
  lcd.print("solid");
}

/***********************************
*            DISPENSING          
***********************************/

void dispenseLiquid() {
  double tbls;
  switch(liqMeasureIdx) {
    //Cups
    case 0:
      beginLiquidDispenseCups(liqAmount);
    break;
    
    //Spoons
    case 1:
      tbls = tblsEquiv[(int)liqAmount];
      beginLiquidDispenseSpoons(tbls);
    break;
  }
}

void dispenseSolid() {
  beginSolidDispenseSpoons(solAmount);
}

void beginLiquidDispenseCups(double amountIn) {
  if(liquidActive) {
    double dispenseAmount = amountIn;
    if(!pumping) {
      startTimingLiquid();
      calculateEndTimeLiquidCups(dispenseAmount);
      digitalWrite(pumpControlPin, HIGH);
    } else {
      if(millis() - liqStartTime >= liqEndTime) {
        pumping = false;
        digitalWrite(pumpControlPin, LOW);
        liquidActive = false;
      }
    }
  } else {
    digitalWrite(pumpControlPin, LOW);
  }
}

void beginLiquidDispenseSpoons(double amountIn) {
  if(liquidActive) {
    double dispenseAmount = amountIn;
    if(!pumping) {
      startTimingLiquid();
      calculateEndTimeLiquidSpoons(dispenseAmount);
      digitalWrite(pumpControlPin, HIGH);
    } else {
      if(millis() - liqStartTime >= liqEndTime) {
        pumping = false;
        digitalWrite(pumpControlPin, LOW);
        liquidActive = false;
      }
    }
  } else {
    digitalWrite(pumpControlPin, LOW);
  }
}

void beginSolidDispenseSpoons(double amountIn) {
  if(solidActive) {
    double dispenseAmount = amountIn;
    if(!stimulating) {
      startTimingSolid();
      calculateEndTimeSolidSpoons(dispenseAmount);
      digitalWrite(stimulatorPin, HIGH);
    } else {
      if(millis() - solStartTime >= solEndTime) {
        stimulating = false;
        digitalWrite(stimulatorPin, LOW);
        solidActive = false;
      }
    }
  } else {
    digitalWrite(pumpControlPin, LOW);
  }
}

void startTimingLiquid() {
  liqStartTime = millis();
  pumping = true;
}

void startTimingSolid() {
  solStartTime = millis();
  stimulating = true;
}

void calculateEndTimeLiquidCups(double amountIn) {
  double duration = (amountIn * 237)/(1.21);
  Serial.println(duration);
  duration = duration * 1000;
  liqEndTime = duration;
  Serial.println(liqEndTime);
}

void calculateEndTimeLiquidSpoons(double amountIn) {
  double duration = (amountIn * 14)/(1.21);
  duration = duration * 1000;
  Serial.println(duration);
  liqEndTime = duration;
  Serial.println(liqEndTime);
}

void calculateEndTimeSolidSpoons(double amountIn) {
  double duration = (amountIn * 140)/(1.21);
  duration = duration * 1000;
  solEndTime = duration;
}

