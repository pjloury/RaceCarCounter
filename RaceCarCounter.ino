/* Philip Loury 2013
 
 The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 
Credits to
 David A. Mellis
 Limor Fried (http://www.ladyada.net)
 Tom Igoe
*/

// include the library code:
#include <LiquidCrystal.h>
#include <NewPing.h>

//Pins
const int red = 6;
const int yellow1 = 7;
const int yellow2 = 8;
const int green = 9;
const int speakerPin = 10;
const int buttonPin2 = 11;
const int buttonPin = 12;
const int pingPin = 13; 
const int rightLed = A0; 
const int leftLed = A1;
const int inPin = A2; //pin which delivers time to receive echo using pulseIn()

int buttonState = 1;
int buttonState2 = 1;

LiquidCrystal lcd(5, 4, 3, 2, A4, A5);

// Music Variables
int songLength = 4; // the number of notes
char notes[] = "cccC "; // a space represents a rest
int beats[] = {2, 2, 2, 3 };
int tempo = 400;
boolean firstTime = true;

// LED Variables;
int leds[] = {red,yellow1,yellow2,green};
int ledLength = 4;

// Timer Variables
double start;
double stop;
boolean raceMode = false;
double lapTimes[10];

boolean refresh=true;
//int triggerRef;
int lapCounter = 1;
double previousLap = 0;
double thisLap;
double lapTime;
double previousLapTime;
double bestLap;
double trackRecordLap = 5.00;

//range in cm which is considered safe to enter, anything 
//coming within less than 5 cm triggers red LED
int rightTrack = 7;
int leftTrack = 13;
//right and Lef LED pin numbers

int flash = 0;
double fastestPossible = 0.75;

#define TRIGGER_PIN  13  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     A2  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 100 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

void setup() {
 
  Serial.begin(115200);

  pinMode(red,OUTPUT); //6
  pinMode(yellow1,OUTPUT); //7
  pinMode(yellow2,OUTPUT); //8
  pinMode(green,OUTPUT); //9
  pinMode(speakerPin, OUTPUT); //10
  pinMode(buttonPin,INPUT); //11
  pinMode(buttonPin2,INPUT); //12
  
  pinMode(pingPin, OUTPUT); //13
  pinMode(rightLed, OUTPUT); //A0
  pinMode(leftLed, OUTPUT); //A1
  pinMode(inPin, INPUT);  //A2
  
  lcd.begin(16, 2);
  lcd.setCursor(1,0);
  lcd.print("Press Play to");
  lcd.setCursor(5,1);
  lcd.print("Begin!");
}

void countTone(){
  delay(100);
  if(firstTime){
    int countDown = 3;
    int ledIndex = 0;

    for (int i = 0; i < songLength; i++) {
      if (notes[i] == ' ') {
        delay(beats[i] * tempo); // rest
      } 
      else {
        lcd.setCursor(3,0);
        lcd.print("Get Ready!");
        lcd.setCursor(6,1);
        if(countDown>0) lcd.print(countDown);
        else lcd.print("GO!");
            

        digitalWrite(leds[ledIndex],HIGH);       
        countDown--;     
        ledIndex++;
        playNote(notes[i], beats[i] * tempo);       
      }
      // pause between notes
      delay(tempo / 2); 
    }
    firstTime = false;
  }
}

void playNote(char note, int duration) {
  char names[] = { 
    'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C'         };
  int tones[] = { 
    1915, 1700, 1519, 1432, 1275, 1136, 1014, 956         };

  // play the tone corresponding to the note name
  for (int i = 0; i < 8; i++) {
    if (names[i] == note) {
      playTone(tones[i], duration);
    }
  }
}

void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(tone);
  }
}

double getTime(){
  return millis()/1000.0;
}

void victory(){
}


void loop() {

  buttonState = digitalRead(buttonPin);
  buttonState2 = digitalRead(buttonPin2);

  if(buttonState == LOW){   // THIS USED TO BE LOW
    lapCounter = 1;
    firstTime = true;
    for(int i=0; i<ledLength;i++){
      digitalWrite(leds[i],LOW);
    }

    lcd.clear();
    lcd.setCursor(3,0);
    lcd.print("Get Ready!");
    countTone();
    start = getTime();//grabs the time at which
    raceMode = true;
    refresh = true; 
    //    startTime();

  }
 
 
 if(raceMode){
    if(refresh)lcd.clear(); 
    if(flash%3>0){
    lcd.setCursor(0,0);    
    lcd.print("Time: ");
    lcd.print(getTime()-start);
    }
    
    pong();

    
    refresh = false;
                                //delay(10);
  flash++;
  if(flash>99)flash=0;  
  }//end raceMode 
}//end loop

void rightLapCount(){
  
  lcd.setCursor(0,1);
  lcd.print("Lap ");
  lcd.print(lapCounter);
  lcd.print(": ");
  
   if(lapCounter == 1)previousLap = start; //previousLap in MILLIS TIME
      thisLap = getTime();
      lapTime = thisLap - previousLap;
      previousLap = thisLap;
      //store the Fastest Lap this Run
      if(lapTime < previousLapTime)bestLap = lapTime;
      previousLapTime = lapTime; 
      lcd.print(lapTime);
      
      //Check for Best Ever
      if(lapTime<trackRecordLap)victory();
      
      //At End of Race      
      if(lapCounter >= 10){
        lcd.clear();
        double stop = getTime();
        double total = stop-start;
        lcd.home();
        lcd.print("Time: ");
        lcd.print(total);
        lcd.setCursor(0,1);
        lcd.print("Best Lap: ");
        //Case for Broken Record       
        if(bestLap<trackRecordLap){
          for(int i =0; i<10; i++){
          lcd.clear();
          lcd.print("Time: ");
          lcd.print(total);
          lcd.setCursor(0,1);
          lcd.print("Best Lap: ");
          lcd.print(bestLap);
          delay(500);
          lcd.clear();
          lcd.print("Time: ");
          lcd.print(total);
          lcd.setCursor(0,1);
          
          delay(250);}  }
        else{lcd.print(bestLap);
          
        }
        raceMode = false;
      }//end of At End of Race
      
      //delay(100);     
      lapCounter++;     
      //digitalWrite(rightLed,LOW);
}

void pong(){
  delay(29);                      // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  unsigned int uS = sonar.ping(); // Send ping, get ping time in microseconds (uS).
  Serial.print("Ping: ");
  int d = uS / US_ROUNDTRIP_CM;
  Serial.print(d); // Convert ping time to distance and print result (0 = outside set distance range, no ping echo)
  Serial.println("cm");
  if(d<=60 && d>=1)digitalWrite(rightLed,HIGH);
  else digitalWrite(rightLed,LOW);
  if(d<=60 && d>=1 && (getTime()-previousLap)>fastestPossible){
    //digitalWrite(rightLed,HIGH);
    rightLapCount();
  }
}

void usr(){
  //raw duration in milliseconds, cm is the 
  //converted amount into a distance
  long duration, cm;
 
  //sending the signal, starting with LOW for a clean signal
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);

  //setting up the input pin, and receiving the duration in 
  //microseconds for the sound to bounce off the object infront
  duration = pulseIn(inPin, HIGH);

  // convert the time into a distance
  cm = microsecondsToCentimeters(duration);
  
  //printing the current readings to ther serial display
  //Serial.print(cm), Serial.print("cm"), Serial.println();
  
  //checking if anything is within the safezone
  if (cm < rightTrack)
  {
    digitalWrite(rightLed, HIGH); //red is right!
    digitalWrite(leftLed, LOW);
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("BYAHH!!!");

  }
  else if(cm<leftTrack && cm >rightTrack)
  {
    digitalWrite(leftLed, HIGH); //green is left!
    digitalWrite(rightLed, LOW);
  }
  else{
    digitalWrite(leftLed,LOW);
    digitalWrite(rightLed,LOW);
  }  
  delay(100);
}

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}



