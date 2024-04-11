#include <LiquidCrystal.h>
#include <Servo.h>

// Constants
// game time (seconds)
int gameTime = 120;
int degrees = 90;
int lowerPoints = 10;
int higherPoints = 20;
int normalDelay = 3000;

// struct to store data for each object
struct StateObject{
  int previousState;
  int counter;
  int currentState;
  int id;
};

// pins for lcd screen
const int rs = 12, en = 11, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int servoPin = 8;
int buttonPin = 2;

// declares servo
Servo servo1;

// declares objects
struct StateObject button1;
struct StateObject motionSensor1;
struct StateObject motionSensor2;

// class to store game data
class Game 
{ 
   public: 
    int gameState; 
	int pointCounter;
  	int timeLeft;
      
    // printname is not defined inside class definition 
  void init(){
    gameState = 1;
    pointCounter = 0;
  }
  
  void addPoints(int a){
    pointCounter += a;
  }
  
  void greetPlayer(){
    // message to play when game starts
    lcd.setCursor(0, 0);
    lcd.print("  Game Started  ");
    lcd.setCursor(0, 1);
    lcd.print("  points:       ");
    lcd.print(pointCounter);
  }
  
  void startGame(){
    lcd.clear();
    init();
    greetPlayer();
    servo1.write(degrees);
  }
  
  void gameRunning(){
    
    char line1[100];
    char line2[100];
  
    lcd.setCursor(0, 0);
    
    sprintf(line1,"  timer:  %01lu:%02lu",((millis() - 3000)/1000)/60,((millis() - 3000)/1000)%60);
    sprintf(line2,"  points: %d  ",pointCounter);
    
    lcd.print(line1);
    lcd.setCursor(0, 1);
    lcd.print(line2);
    
    if((millis() - 3000) >= (gameTime * 1000)){
      lcd.clear();
      gameState = 0;
    }
  }
  
  void endGame(){
    char line1[100];
    char line2[100];
    sprintf(line1,"  GAME OVER     ");
    sprintf(line2,"  SCORE: %d     ",pointCounter);
	
    lcd.print(line1);
    lcd.setCursor(0, 1);
    lcd.print(line2);
    
    servo1.write( -1 * degrees);
  }
  
}; 


// declares game
Game game1;

void setup()
{
  // sets up servo
  servo1.attach(servoPin);
  
  // sets pin 2 to read the button
  pinMode(2, INPUT);
  
  // sets the serial to read at 9600 bits/second
  Serial.begin(9600);
  
  // Sets up LCD as 16 x 2
  lcd.begin(16, 2);
  
  // initializes button 1
  button1.counter = 0;
  button1.previousState = 0;
  button1.currentState = 0;
  button1.id = 0;
  
  // initializes motionSensor 1
  motionSensor1.counter = 0;
  motionSensor1.previousState = 0;
  motionSensor1.currentState = 0;
  motionSensor1.id = 1;
  
  // initializes motionsensor 2
  motionSensor2.counter = 0;
  motionSensor2.previousState = 0;
  motionSensor2.currentState = 0;
  motionSensor2.id = 2;
 
  game1.gameState = -1;
  lcd.clear();
}


// takes an object and "does" a function whenever it's triggered
void stateManager( struct StateObject *s, void (*functionPtr)(int, void*) , void *p ){
  if( (s->previousState) != ( s->currentState )){
    s->previousState = s->currentState;
    s->counter = (s->counter) + 1;
    
    if((s->counter % 2) == 0){
    	(*functionPtr)(s->id,p);
  	}
    
  }
  
}


// function after button is pressed (p is a void pointer) could be used for anything)
void buttonAction(int id, void *p){
  Serial.print("buttonPressed:");
  Serial.println(id);
  if(game1.gameState == 1){
    lcd.clear();
    game1.endGame();
    game1.gameState = 0;
    delay(normalDelay);
    lcd.clear();
    
  }else if(game1.gameState == 0){
  	game1.startGame();
    delay(normalDelay);
  }else{
    game1.startGame();
    delay(normalDelay);
  }
  
}

// function after motion is detected
void motionSensorAction(int id, void *p){
  Serial.println("motion sensed at: ");
  Serial.println(id);
  
  if( id == 1){
   	Game *g = static_cast<Game*>(p);
    g->pointCounter += lowerPoints;
  }
  
  if( id == 2){
   	Game *g = static_cast<Game*>(p);
    g->pointCounter += higherPoints;
  }
  
}

void loop()
{
  // button logic
  /* -----------------------------------*/
  button1.currentState = digitalRead(buttonPin);
  stateManager( (&button1), &buttonAction, &game1);
  
  // motionSensor logic
  /*------------------------------------*/
  motionSensor1.currentState = digitalRead(10);
  stateManager( (&motionSensor1), &motionSensorAction, &game1);
  
  motionSensor2.currentState = digitalRead(9);
  stateManager( (&motionSensor2), &motionSensorAction, &game1);
  if(game1.gameState == 1){
    game1.gameRunning();
  }else if(game1.gameState == 0){
    // game over logic
  	game1.endGame();
  }
  
}
