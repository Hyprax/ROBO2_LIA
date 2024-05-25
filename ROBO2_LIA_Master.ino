// Include Arduino Wire library for I2C
#include <Wire.h>
#include <IRremote.hpp>
#include <Servo.h>
#include "FastLED.h"

//Motor A
// Here we define pin 8 of Arduino as the motor driver's Input connection for motors A.
#define INA 8
// Here we define pwm pin 5 of Arduino as the motor driver's enable connection for motors A.
#define PWMA 5

//Motor B
// Here we define pin 7 of Arduino as the motor driver's Input connection for motors B.
#define INB 7
// Here we define pwm pin 6 of Arduino as the motor driver's enable connection for motors B.
#define PWMB 6

//STBY
// Here we define pin 3 of Arduino as the standby connection of the motor driver.
#define STBY 3

//Direction
// Here we define clockwise as 1, later used with digital.write (1 = HIGH).
#define clockWise 1
// Here we define anticlockwise as 0, later used with digital.write (0 = LOW).
#define antiClockwise 0
#define ahead 90
#define right 25
#define left 155

//Servo
// Here we create servo object to control a servo.
Servo myservo;

//Ultrasonic
#define Trigger 13
#define Echo 12
float duration, distance;
bool SonicSensor = true;

//IR
#define IReceive 9

//Remote Inputs
#define UP    70
#define RIGHT 67
#define LEFT  68
#define DOWN  21
#define OK    64
#define STAR  66
#define HSHTG 74  
#define Zero  82
#define One   22
#define Two   25

//I2C 
#define SlaveAddress 10
#define AnswerSize 8

//RGB
#define PIN_RBGLED 4
#define NUM_LEDS 1
CRGB leds[NUM_LEDS];

uint32_t Color(uint8_t r, uint8_t g, uint8_t b)
{
  return (((uint32_t)r << 16) | ((uint32_t)g << 8) | b);
}

//Time
unsigned long PreviousTime = 0;
unsigned long CurrenTime = millis;
const long StopDelay = 5000;
 
void setup() 
{
//COMs
  Serial.begin(9600);
//I2C
  Wire.begin();
//Receiver
  IrReceiver.begin(IReceive, ENABLE_LED_FEEDBACK);
//Motors
  pinMode(INA, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(INB, OUTPUT);
  pinMode(PWMB, OUTPUT);
  digitalWrite(STBY, 1);
//Servo motor
  myservo.attach(10);
//RGB
  FastLED.addLeds<NEOPIXEL, PIN_RBGLED>(leds, NUM_LEDS);
  FastLED.setBrightness(20);
//UltSonic
  pinMode(Trigger, OUTPUT);
  pinMode(Echo, INPUT);
//Distances
  float DisFw = 0;
  float DisRt = 0; 
  float DisLt = 0;
}
 
void loop() 
{
  if (IrReceiver.decode()) 
 {
    // Print that value on the Serial Monitor.
    Serial.println(IrReceiver.decodedIRData.command);
    // and IF the decoded value received corresponds to the OK button then ...
    if(IrReceiver.decodedIRData.command == OK)
    {
      //Stop the motors.
      Stop();
    }
    // IF NOT then check if the value matches with the up arrow button. IF SO then ...
    else if(IrReceiver.decodedIRData.command == UP)
    {
      // both sets of Motors rotate clockwise at a speed of 100. (advance)
      Forward(100);
      // Head(servo) facing straight forward. (95 degrees because for some reason 90 degrees isn't perfectly straight)
      myservo.write(95);
    }
    // IF NOT then check if the value matches with the right arrow button. IF SO then ...
    else if(IrReceiver.decodedIRData.command == RIGHT)
    {
      // Left motors spin clockwise at speed of 60 and right motors spin counterclockwise at the same speed. (turn right) 
      Right(60);
      // Head(servo) facing to the right.
      myservo.write(25);
    }
    // IF NOT then check if the value matches with the left arrow button. IF SO then ...
    else if(IrReceiver.decodedIRData.command == LEFT)
    {
      // Left motors spin counter clockwise at a speed of 60 and right motors spin in other direction at same speed (turn left)
      Left(60);
      // Head(servo) facing to the left.
      myservo.write(160);
    }
    // IF NOT then check if the value matches with the down arrow button. IF SO then ...
    else if(IrReceiver.decodedIRData.command == DOWN)
    {
      // Both sets of motors spin counterclockwise at a speed of 100. (reverse)
      Backward(100);
      // Head(servo) facing straight forward.
      myservo.write(95);
    }
    // This resumes the reading of IR signals. (resume reading and go back through the loop)
    IrReceiver.resume();
 }
 ReceiveEvent();
}

//Functions

//I2C
void SendEvent()
{
  Serial.println("Write data to slave");
  Wire.beginTransmission(SlaveAddress);
  Wire.write(1);
  Wire.endTransmission();
}

void ReceiveEvent()
{ 
  Serial.println("Receive data");

  Wire.requestFrom(SlaveAddress,AnswerSize);
  // Add characters to string
  String response = "";
  while (Wire.available()) 
  {
    char b = Wire.read();
    response += b;
  } 
  
  // Print to Serial Monitor
  Serial.println(response);

  if(response == "Criminal")
  {
    FollowMode();
    Serial.println("POLICE CHASE");
    /*
    int r=255,g=0,b=0;
    FastLED.showColor(Color(r, g, b));
    delay(200);
    r=0,g=0,b=255;
    FastLED.showColor(Color(r, g, b));
    delay(200);
    */
  }
}

int SonicSens()
{
  if(SonicSensor == true)
  {
    digitalWrite(Trigger, LOW);
    delayMicroseconds(2);
    digitalWrite(Trigger, HIGH);
    delayMicroseconds(20);
    digitalWrite(Trigger, LOW);

    duration = pulseIn(Echo, HIGH);
    distance = (duration / 58);
    return (int)distance;
  }
  else if(SonicSensor == false)
  {
    digitalWrite(Trigger, LOW);
    distance = 0;
    return (int)distance;
  }
}

// This is my function for the forward movement.
void Forward(int speed)
{
//Motors A
  /*
  Here we send voltage to the input connection of the motor driver which is an H bridge.
  This allows the motors to have a certain polarity which affects the direction at which the motors spin.
  In this case we make motors A spin clockwise. (HIGH = input 1 on, input 2 off) The shield inverses the state of the other input so we don't have to.
  So in summary: HIGH/1 = clockwise and LOW/0 = counterclockwise.
  */
  // make motors A spin clockwise.
  digitalWrite(INA, clockWise);
  // control the speed with whatever bit value the operator sets inside the parentheses of function. 
  analogWrite(PWMA, speed);
//Motors B
  // make motors B spin clockwise.
  digitalWrite(INB, clockWise);
  // control the speed with whatever bit value the operator sets inside the parentheses of function.
  analogWrite(PWMB, speed);
}

// This is my function for the turn right movement.
void Right(int speed)
{
//Motors A
  // make motors A spin clockwise.
  digitalWrite(INA, clockWise);
  // control the speed with whatever bit value the operator sets inside the parentheses of function.  
  analogWrite(PWMA, speed);
// Motors B
  // make motors B spin anticlockwise.
  digitalWrite(INB, antiClockwise);
  // control the speed with whatever bit value the operator sets inside the parentheses of function.
  analogWrite(PWMB, speed);
}

// This is my function for the turn left movement.
void Left(int speed)
{
//Motors A
  // make motors A spin anticlockwise.
  digitalWrite(INA, antiClockwise);
  // control the speed with whatever bit value the operator sets inside the parentheses of function. 
  analogWrite(PWMA, speed);
//Motors B
  // make motors B spin clockwise.
  digitalWrite(INB, clockWise);
  // control the speed with whatever bit value the operator sets inside the parentheses of function.
  analogWrite(PWMB, speed);
}

// This is my function for the reverse movement.
void Backward(int speed)
{
//Motors A
  // make motors A spin anticlockwise.
  digitalWrite(INA, antiClockwise);
  // control the speed with whatever bit value the operator sets inside the parentheses of function.
  analogWrite(PWMA, speed);
//Motors B
  // make motors B spin anticlockwise.
  digitalWrite(INB, antiClockwise);
  // control the speed with whatever bit value the operator sets inside the parentheses of function.
  analogWrite(PWMB, speed);
}

// This is my function for the stop.
void Stop()
{
//Motors A
  // Here the direction does not really matter but the speed is always set to 0.
  digitalWrite(INA, clockWise);
  // Speed set to 0 (stationary)
  analogWrite(PWMA, 0);
//Motors B
  // Same for motors B.
  digitalWrite(INB, clockWise);
  // Speed set to 0 (stationary)
  analogWrite(PWMB, 0);
}

void FollowMode()
{
      myservo.write(ahead);
      delay(500);
      SonicSens();
      float DisFw = distance;
      if(distance >= 30)
      {
        myservo.write(right);
        delay(500);
        SonicSens();
        float DisRt = distance;
        if(distance >= 30)
        {
          myservo.write(left);
          delay(500);
          SonicSens();
          float DisLt = distance;
          if(distance >= 30)
          {
            if(DisFw >= DisRt)
            {
              if(DisRt > DisLt)
              {
                Left(100);
                delay(500);
                Forward(100);
              }
              else
              {
                Right(100);
                delay(500);
                Forward(100);
                myservo.write(ahead); //maybe remove
              }
            }
            else
            {
              if(DisFw > DisLt)
              {
                Left(100);
                delay(500);
                Forward(100);
                myservo.write(ahead); //maybe remove
              }
              else
              {
                Forward(100); 
              }
            }
          }
          else if(distance < 30 && distance > 15)
          {
            Left(100);
            delay(250);
            myservo.write(ahead);
            Forward(100);
          }
        }
        else if(distance < 30 && distance > 15)
        {
          Right(100);
          delay(250);
          myservo.write(ahead);
          Forward(100);
        }
      }
      else if(distance < 30 && distance > 15)
      {
        Forward(100);
      }
      if(distance >= 10 && distance <= 15)
      {
        Stop();
        if(CurrenTime - PreviousTime >= StopDelay)
        {
          PreviousTime = CurrenTime;
          SonicSensor = false;
        }
      }
      else if(distance < 10 && distance > 0)
      {
        Backward(100);
      }
    if(SonicSensor == false || distance == 0)
    {
      Backward(100);
      delay(500);
      Right(100);
      delay(500);
      Forward(100);
      delay(500);
    }
}
