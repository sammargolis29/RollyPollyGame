#include <Wire.h> // Must include Wire library for I2C
#include <SparkFun_MMA8452Q.h> // Includes the SFE_MMA8452Q library
MMA8452Q accel;
enum State {
  flat, //0
  left, //1
  right, //2
  up, //3
  down, //4
};
State orientation = flat;
void setup ()
{
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(2, OUTPUT);

  digitalWrite(13, HIGH); //COL 5
  digitalWrite(12, HIGH); //COL 4
  digitalWrite(11, HIGH); //COL 3
  digitalWrite(10, HIGH); //COL 2
  digitalWrite(9, HIGH); //COL 1
  digitalWrite(8, LOW); //ROW 7
  digitalWrite(7, LOW); //ROW 6
  digitalWrite(6, LOW); //ROW 5
  digitalWrite(5, LOW); //ROW 4
  digitalWrite(4, LOW); //ROW 3
  digitalWrite(3, LOW); //ROW 2
  digitalWrite(2, LOW); //ROW 1
  accel.init();
}

unsigned long predChange = 0;
unsigned long predDelay = 125;
bool blink = false;

unsigned long Preydel = 150;
unsigned long preyChange = 0;

int preyX = 1;
int preyY = 1;

int predX = 3;
int predY = 3;

int key = 0;

int preyWins = 0;
int predWins = 0;
int rounds = 0;
int winLoss = 0;

bool caught = false;

unsigned long elapsedTime = 0;


void loop()
{
  // Use the accel.available() function to wait for new data
  //  from the accelerometer.
  if (accel.available())
  {
    accel.read();
    orientation = accelOrientation(orientation);
  }
  prey(orientation);
  predator();
  displayPrey();
  caught = preyCaught();
  unsigned long now = millis();
  if (blink == false) {
    displayPredator();
  }
  
  if(now>=predChange&&blink==false){
     blink = true;
    predChange += predDelay;
  }
  
  if (now >= predChange && blink == true) {
    clearDisplay();
    blink = false;
    predChange += predDelay;
  }

  if(millis() - elapsedTime > 15000 || caught == true){
    if(caught == true){
      predWins += 1;
      winLoss = predWins - preyWins;
      displayWinLoss();
    }

    else {
      preyWins += 1;
      winLoss = preyWins - predWins;
      displayWinLoss();
    } 
  }
}

void displayWinLoss(){
  unsigned long scoreDel = 4000;
  scoreDel += millis();
  while(millis() <= scoreDel){
    clearDisplay();
    if(caught == true){ //predator won
      predX = 3;
      predY = 4;
      displayPredator();
      delay(1);
      clearDisplay();    
    }
  
    else if(caught == false){ //prey won
      preyX = 3;
      preyY = 4;
      displayPrey();
    }

    digitalWrite(8, HIGH);
    
    if(winLoss < -2){
        digitalWrite(9, LOW);
        digitalWrite(10, LOW);
        digitalWrite(11, LOW);
    }

    else if(winLoss < 0){
        digitalWrite(10, LOW);
        digitalWrite(11, LOW);
    }

    else if(winLoss == 0){
      digitalWrite(11, LOW);
    }
    
    else if(winLoss < 3){
      digitalWrite(11, LOW);
      digitalWrite(12, LOW);
    }

    else{
      digitalWrite(11, LOW);
      digitalWrite(12, LOW);
      digitalWrite(13, LOW);
    }
    delay(1);
   
  }
  
  caught = false;
  elapsedTime = millis();
  Serial.println(elapsedTime);
  preyX = 1;
  predY = 1;
  predX = 3;
  predY = 3;
  clearDisplay();
}

void clearDisplay(){
  digitalWrite(13, HIGH); //COL 5
  digitalWrite(12, HIGH); //COL 4
  digitalWrite(11, HIGH); //COL 3
  digitalWrite(10, HIGH); //COL 2
  digitalWrite(9, HIGH); //COL 1
  digitalWrite(8, LOW); //ROW 7
  digitalWrite(7, LOW); //ROW 6
  digitalWrite(6, LOW); //ROW 5
  digitalWrite(5, LOW); //ROW 4
  digitalWrite(4, LOW); //ROW 3
  digitalWrite(3, LOW); //ROW 2
  digitalWrite(2, LOW); //ROW 1
}

unsigned long moveDelay=500;
unsigned long moveTime=0;

void predator() {
  unsigned long now=millis();
  if (Serial.available() > 0 && now >= moveTime) {
    displayPredatorOff();
    key = Serial.read();
    if (key == 119) { //up
      Serial.print("w");
      if (predY != 1) {
        predY -= 1;
      }
    }
    
    else if (key == 115) { //down
      Serial.print("s");
      if (predY != 6) {
        predY += 1;
      }
    }
    
    else if (key == 97) { //left
      Serial.print("a");
      if (predX != 1) {
        predX -= 1;
      }
    }
    
    else if (key == 100) { //right
      Serial.print("d");
      if (predX != 4) {
        predX += 1;
      }
    }
    moveTime = now + moveDelay;
  }
}

boolean preyCaught(){
  if(preyX == predX && (preyY == predY || preyY == (predY + 1)) || (preyX == (predX + 1) && (preyY == predY || preyY == (predY + 1)))){
      return true;
  }
   
  return false;
}

void prey(State orientation) {
  unsigned long now = millis();
  if (now >= preyChange) {
    if (orientation == 0) { //flat
    }
    
    else if (orientation == 1) { //left
      if (preyX != 1) {
        preyX -= 1;
      }
    }
    
    else if (orientation == 2) { //right
      if (preyX != 5) {
        preyX += 1;
      }
    }

    else if (orientation == 3) { //down
      if (preyY != 7) {
        preyY += 1;
      }
    }

    else if (orientation == 4) { //up
      if (preyY != 1) {
        preyY -= 1;
      }
    }
    
    preyChange = now + Preydel;
  }
}

void displayPredator() {
  digitalWrite(predX + 8, LOW); //topleft
  digitalWrite(predY + 1, HIGH);

  digitalWrite(predX + 9, LOW); //topright
  digitalWrite(predY + 1, HIGH);

  digitalWrite(predX + 8, LOW); //bottomleft
  digitalWrite(predY + 2, HIGH);

  digitalWrite(predX + 9, LOW); //bottomright
  digitalWrite(predY + 2, HIGH); 
}

void displayPredatorOff() {
  digitalWrite(predX + 8, HIGH); //topleft
  digitalWrite(predY + 1, LOW);

  digitalWrite(predX + 9, HIGH); //topright
  digitalWrite(predY + 1, LOW);

  digitalWrite(predX + 8, HIGH); //bottomleft
  digitalWrite(predY + 2, LOW);

  digitalWrite(predX + 9, HIGH); //bottomright
  digitalWrite(predY + 2, LOW);
}

void displayPrey() {
  displayPredatorOff();
  digitalWrite(preyX + 8, LOW);
  digitalWrite(preyY + 1, HIGH);
  delay(1);
  digitalWrite(preyX + 8, HIGH);
  digitalWrite(preyY + 1, LOW);
}
//push
State accelOrientation(State state)
{
  // accel.readPL() will return a byte containing information
  // about the orientation of the sensor. It will be either
  // PORTRAIT_U, PORTRAIT_D, LANDSCAPE_R, LANDSCAPE_L, or
  // LOCKOUT.
  byte pl = accel.readPL();
  switch (pl)
  {
    case PORTRAIT_U:
      //Serial.print("Portrait Up");
      state = up;
      break;
    case PORTRAIT_D:
      //Serial.print("Portrait Down");
      state = down;
      break;
    case LANDSCAPE_R:
      //Serial.print("Landscape Right");
      state = right;
      break;
    case LANDSCAPE_L:
      //Serial.print("Landscape Left");
      state = left;
      break;
    case LOCKOUT:
      //Serial.print("Flat");
      state = flat;
      break;
  }
  return state;
}
