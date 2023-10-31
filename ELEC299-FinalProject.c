/**
 * @file Final_proj
 * @author Will Hurley, Jason Debbo, Michael Borg
 * Program containing algorithm to navigate the course provided for the 299 final project
 * Has claw access, proximity sensor acess and line following capability
 **
 */

 #include <Servo.h>

// Create a servo object to control the claw
Servo S2;
Servo S1;

// Variable to store each servo position [degrees]
int angle = 0;

 // Arduino analog input pin to which the Sharp sensor is connected
const byte front_PIN = A1;
const byte left_PIN = A0;
const byte right_PIN = A2;

// Variable to store the proximity measurements
int front_val = 0;
int left_val = 0;
int right_val = 0;

//line following pin assignmnents
int r_pin = A3; // Right Sensor on Analog Pin 3
int l_pin = A4; // Left Sensor on Analog Pin 4 
int m_pin = A5; // Middle Sensor on Analog Pin 5

int l_val, r_val, m_val; // Sensor values 

int E1 = 6; //M1 Speed Control
int E2 = 5; //M2 Speed Control
int M1 = 7; //M1 Direction Control
int M2 = 4; //M2 Direction Control

const int white_lvl = 600; // reading level is white if <600
const int black_lvl = 800; // reading level is black if >800

//Control Signals
int speed = 140;
int turnspeed = 255;

//Counters to select proper loop
int counter = 0;
int fcounter = 0;

void setup() {
  Serial.begin(9600);
  //set up servo pin assginmentss for claw
  S2.attach(12);
  S1.attach(11);
}

void loop() {


  // read line following sensors  
  readsensors();
    
  // Case 1: Left and right are reading white and middle is reading black.
  if (l_val < white_lvl && m_val > black_lvl && r_val < white_lvl) {
    goforward();
  }

  // Case 2 : Left and middle are reading white and right is reading black.
  else if (l_val < white_lvl && m_val< white_lvl && r_val > black_lvl) {
    turnright();


    while(true) { 
      readsensors();
      if ((l_val < white_lvl && m_val > black_lvl && r_val < white_lvl) 
          || (l_val > black_lvl && m_val > black_lvl && r_val < white_lvl)) 
      {break;} // Break if right is reading white and middle is reading black
    }
  }
   
  // Case 3 : Left is reading white, middle and right are reading black.
  else if (l_val < white_lvl && m_val > black_lvl && r_val > black_lvl) {
    turnright();

    
    while(true) { 
      readsensors();
      if ((l_val < white_lvl && m_val > black_lvl && r_val < white_lvl) 
          || (l_val > black_lvl && m_val > black_lvl && r_val < white_lvl))
      {break;}  // Break if right is reading white and middle is reading black
    }
  }

  // Case 4 : Left is reading black, middle and right are reading white. 
  else if (l_val > black_lvl && m_val < white_lvl && r_val < white_lvl) {
    turnleft();

    
    while(true) { 
      readsensors();
      if ((l_val < white_lvl && m_val > black_lvl && r_val < white_lvl) 
          || (l_val< white_lvl && m_val > black_lvl && r_val > black_lvl))
      {break;}  // Break if left is reading white and middle is reading black
    }
  }

  // Case 5 : Left and middle are reading black and right is reading white. 
  else if (l_val > black_lvl && m_val> black_lvl && r_val < white_lvl) {
    turnleft();

    
     while(true) { 
      readsensors();
      if ((l_val < white_lvl && m_val > black_lvl && r_val < white_lvl ) || (l_val< white_lvl && m_val > black_lvl && r_val > black_lvl))
      {break;} // Break if left is reading white and middle is reading black
    }
  }

  //Case 6: All read black and counter = 0. Stop, read sensors and turn to clear path.
  else if (l_val > black_lvl && m_val> black_lvl && r_val > black_lvl && counter == 0) {
    counter = 1;//increments counter so that this doesnt get accessed again
    delay(200);
    stoprover();//stop and allow sensors using check barrier function
    delay(2000);
    checkbarriers();

  }

  //Case 7: All read black and counter = 1. Stop and perform flag functions. 
  else if (l_val > black_lvl && m_val> black_lvl && r_val > black_lvl && counter == 1){
    //once counter is 1 we want to either pickup or putdown flag
    stoprover();
    counter = 0;// update counter so upon return it will do the above function to select the proper path

    //Pickup flag
    if (fcounter == 0){
        pickUp(); //counter back to zero, 180 spin
        turnright();
        delay(1000);
        fcounter = 1;
    //Drop flag
    }else if (fcounter == 1){

        //Spin 180
        turnright();
        delay(1000);
        
        //Drop flag     
         putDown();

        //Reverse into garage
       goreverso();
       delay(1000);
    }  
  }
}

void checkbarriers(){
  // Read the sensor output (0-1023, or 10 bits)
    front_val = analogRead(front_PIN);
    left_val = analogRead(left_PIN);
    right_val = analogRead(right_PIN);
    
    //checks each case by comparing values
  if(front_val > right_val && left_val > right_val){
    //turn to open path
    turnright();
    delay(500);
  }
  else if(front_val > left_val && right_val > left_val){
    //turn to open path
    turnleft();
    delay(500);
  }
  else if(left_val > front_val && right_val > front_val){
     //go through cross
     goforward();
     delay(500);
  }
}
  

// Go Forward Routine 
void goforward() {
  analogWrite (E1,speed);
  digitalWrite(M1,LOW);
  analogWrite (E2,speed);
  digitalWrite(M2,HIGH);
}
//go reverse
void goreverse() {
  analogWrite (E1,speed);
  digitalWrite(M1,HIGH);
  analogWrite (E2,speed);
  digitalWrite(M2,LOW);
}

// Turn Right Routine 
void turnright() {
  analogWrite (E1,turnspeed);
  digitalWrite(M1,LOW);
  analogWrite (E2,turnspeed);
  digitalWrite(M2,LOW);
}

// Turn Left Routine
void turnleft() {
  analogWrite (E1,turnspeed);
  digitalWrite(M1,HIGH);
  analogWrite (E2,turnspeed);
  digitalWrite(M2,HIGH);
}
//Stops rover
void stoprover(){
  analogWrite (E1,0);
  digitalWrite(M1,HIGH);
  analogWrite (E2,0);
  digitalWrite(M2,HIGH);
}

// Read all line following sensors
void readsensors() {
  l_val = analogRead(l_pin);

  r_val = analogRead(r_pin);

  m_val = analogRead(m_pin);
}

//put down flag
void putDown(){
    // For angle 180 to 0 [degrees]
    for (angle = 180; angle >= 90; angle -= 1)
    {
        // Tell servo to go to position "angle"
        S2.write(angle);

        // Wait a specified time [ms]
        delay(100);
    }

    // For angle 180 to 0 [degrees]
    for (angle = 180; angle >= 90; angle -= 1)
    {
        // Tell servo to go to position "angle"
        S1.write(angle);

        // Wait a specified time [ms]
        delay(100);
    }
}
// pick up flag
void pickUp(){

    // For angle 0 to 180 [degrees]
    for (angle = 90; angle <= 180; angle += 1)
    {
        // Tell servo to go to position "angle"
        S1.write(angle);

        // Wait a specified time [ms]
        delay(100);
    }

    // For angle 0 to 180 [degrees]
    for (angle = 90; angle <= 180; angle += 1)
    {
        // Tell servo to go to position "angle"
        S2.write(angle);

        // Wait a specified time [ms]
        delay(100);
    }
}