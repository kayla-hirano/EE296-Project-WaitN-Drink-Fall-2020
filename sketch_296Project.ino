//Include libraries
#include <Adafruit_AHTX0.h>     
 
Adafruit_AHTX0 aht;

//time variables
long currenttime;
long starttime;
bool initial = true;

//ultrasonic sensor pins
#define trigPin 13
#define echoPin 12


//SENSOR TEMP RANGES
long toohot = 0;
long almost = 0;

//read in value sent from app
char appData;
//converted temp from appData
long temp = 0;
//check for input
bool processing = false;
//check for cup on tray
bool detection = false;

/////////
//Setup//
/////////

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.setTimeout(10);

  if (! aht.begin()) {
    while (1) delay(100);
  }
}

////////
//Loop//
////////

void loop() {
  // put your main code here, to run repeatedly:

  //ultrasonic sensor configuration
  long duration, distance;

  //initial detection 
  while (!detection){
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = (duration/2) / 29.1;
    
    //If cup is on tray, set detection to true
    if(distance < 5){
      detection = true;
      Serial.print("ready");
    }
    delay (1000);
  }
  
  //read in temp value sent from app
  while(!processing){
    while (Serial.available() == 0) {
    }
    appData = Serial.read();
    
    //Convert value recieved from app into actual temp
    temp = convertValue(appData, toohot, almost);
    
    processing = true;
    initial = true;
    starttime = millis();
  }

  //initial delay of four minutes 
  currenttime = millis();
  if((currenttime-starttime) < 240000){
    Serial.print("h");
  }else{
    initial = false;
  }

  while(!initial){
    //read temp sensor 
    sensors_event_t humidity, temps;
    aht.getEvent(&humidity, &temps);// populate temp with fresh data
    long sensetemp = temps.temperature;
    
    //compare the two temperatures
    if(calculateIdeal(sensetemp, temp)){
      Serial.print("d");
      Serial.flush();
      processing = false;
      detection = false;
      initial = true;
    }else{
      if(sensetemp >= toohot){
        Serial.print("h");
      }else{
        Serial.print("a");
      }
    }

    delay(2000);
  }

  delay(2000);

}

/////////////
//FUNCTIONS//
/////////////

bool calculateIdeal(long Value, long temp){
  //if value is ideal temp, return true
  if(Value <= temp){
    return true;
  }else{
    return false;
  }
}

long convertValue(char appData, long &toohot, long &almost){
  //add conversions when needed 
  //Ceramic, Hot
  if(appData == 'a'){
      toohot = 100;
      almost = 45;
      return 43;
  //Ceramic, Warm 
  }else if (appData == 'b'){
      toohot = 45;
      almost = 42;
      return 38;
  //Ceramic, Lukewarm 
  }else if (appData == 'c'){
      toohot = 45;
      almost = 40;
      return 35; 
  //Glass, Hot
  }else if(appData == 'd'){
      toohot = 100;
      almost = 49;
      return 47;
  //Glass, Warm 
  }else if (appData == 'e'){
      toohot = 50;
      almost = 45;
      return 39;
  //Glass, Lukewarm 
  }else if (appData == 'f'){
      toohot = 50;
      almost = 43;
      return 37;
  //Plastic, Hot
  }else if(appData == 'g'){
      toohot = 100;
      almost = 51;
      return 45;
  //Plastic, Warm 
  }else if (appData == 'h'){
      toohot = 51;
      almost = 43;
      return 37;
  //Plastic, Lukewarm 
  }else if (appData == 'i'){
      toohot = 51;
      almost = 39;
      return 35;
  }else{
      return 1000;
  }
}
