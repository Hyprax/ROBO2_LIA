//Libraries
#include <IRremote.hpp>
#include <Servo.h>

//Motor A
#define INA 8
#define PWMA 5

//Motor B
#define INB 7
#define PWMB 6

//STBY
#define STBY 3

//Direction
#define clockWise 1
#define antiClockwise 0

//Servo
Servo myservo;
int position = 0;

//Line Trackers
#define sensorMid A1
#define sensorR A0
#define sensorL A2

//Ultrasonic
#define Trigger 13
#define Echo 12
float duration, distance;

//InfraRed
#define IReceive 9

//Arrows
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

//Time
unsigned long previousTime = 0;
const long prinTime = 150;

//SRCH
int search = 0;

void setup() 
{
//Motors
  pinMode(INA, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(INB, OUTPUT);
  pinMode(PWMB, OUTPUT);
  digitalWrite(STBY, 1);

//Servo Motor
  myservo.attach(10);

//LineSensors
  pinMode(sensorMid, INPUT);
  pinMode(sensorR, INPUT);
  pinMode(sensorL, INPUT);

//UltSonic
  pinMode(Trigger, OUTPUT);
  pinMode(Echo, INPUT);

//Receiver
  IrReceiver.begin(IReceive, ENABLE_LED_FEEDBACK);

//Serial Monitor
  Serial.begin(9600);
}

void loop() 
{
//Control
  if (IrReceiver.decode()) 
  {
    Serial.println(IrReceiver.decodedIRData.command);
    if(IrReceiver.decodedIRData.command == OK)
    {
      Stop();
    }
    else if (IrReceiver.decodedIRData.command == One)
    {
      while (IrReceiver.decodedIRData.command == OK)
      {
        if(IrReceiver.decodedIRData.command == UP)
        {
          Forward(100);
          myservo.write(95);
        }
        else if(IrReceiver.decodedIRData.command == RIGHT)
        {
          ForwardRight(100);
          myservo.write(25);
        }
        else if(IrReceiver.decodedIRData.command == LEFT)
        {
          ForwardLeft(100);
          myservo.write(160);
        }
        break;
      }
    }
    else if (IrReceiver.decodedIRData.command == Zero)
    {
      if(IrReceiver.decodedIRData.command == UP)
      {
        Backward(100);
        myservo.write(95);
      }
      else if(IrReceiver.decodedIRData.command == RIGHT)
      {
        BackwardRight(100);
        myservo.write(25);
      }
      else if(IrReceiver.decodedIRData.command == LEFT)
      {
        BackwardLeft(100);
        myservo.write(160);
      }      
    }
    else if (IrReceiver.decodedIRData.command == Two)
    {
      if(IrReceiver.decodedIRData.command == UP)
      {
        Forward(100);
        myservo.write(95);
      }
      else if(IrReceiver.decodedIRData.command == RIGHT)
      { 
        Right(60);
        myservo.write(25);
      }
      else if(IrReceiver.decodedIRData.command == LEFT)
      {
        Left(60);
        myservo.write(160);
      }
      else if(IrReceiver.decodedIRData.command == DOWN)
      {
        Backward(100);
        myservo.write(95);
      }
    }
    else if (IrReceiver.decodedIRData.command == STAR)
    {
      ObstacleAvoidance();
    }
    else if (IrReceiver.decodedIRData.command == HSHTG)
    {
      LineTracking();
    }
    IrReceiver.resume();
  }
}

//Sensing + etc
void SonicSens()
{
  digitalWrite(Trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trigger, LOW);

  duration = pulseIn(Echo, HIGH);
  distance = (duration/2) * 0.0343;
}

void Nod()
{
  Stop();
  for (position = 0; position <= 180; position += 4) 
  {
    myservo.write(position);
    delay(15);
  }
  for (position = 180; position >= 0; position -= 4) 
  {
    myservo.write(position);
    delay(15);
  }
}

void Search() 
{
  switch (search) 
  {
    case 0:
      myservo.write(25);
      Right(55);
      if (millis() - previousTime > 2500)
      {
        search = 1;
        previousTime = millis();
      }
      break;

    case 1:
      myservo.write(160);
      Left(55);
      if (millis() - previousTime > 5500)
      {
        search = 2;
        previousTime = millis();
      }
      break;

    case 2:
      Stop();
      myservo.write(95);
    
  }
}

//Modes
void ObstacleAvoidance()
{
//Values
  int Rval = analogRead(sensorR);
  int Lval = analogRead(sensorL);
  int midVal = analogRead(sensorMid);

  if(midVal >= 500 && Rval >= 500 && Lval >= 500)
  {
    Stop();
  }
  else
  {
    myservo.write(95);
    SonicSens();
    if (distance < 20)
    {
      Stop();
      myservo.write(0);
      delay(2500);
      SonicSens();
      if(distance < 20)
      {
        Stop();
        myservo.write(180);
        delay(2500);
        SonicSens();
        if(distance < 20)
        {
          Stop();
          Backward(100);
          delay(1000);
          myservo.write(0);
          Right(50);
          delay(2500);
        }
        else
        {
          Left(50);
          delay(1250);  
          myservo.write(95);
          delay(250);
        }
      }
      else
      {
        Right(50);
        delay(1250);
        myservo.write(95);
        delay(250);
      }
    }
    else
    {
      Forward(100);
      myservo.write(95);
    }
  }
}

void LineTracking()
{
//Values
  int Rval = analogRead(sensorR);
  int Lval = analogRead(sensorL);
  int midVal = analogRead(sensorMid);

//Millies
  unsigned long currentTime = millis();

//Print
  if(currentTime - previousTime >= prinTime)
  {
    previousTime = currentTime;
    Serial.print("L: ");
    Serial.print(Lval);
    Serial.print("\t\t");
    Serial.print("M: ");
    Serial.print(midVal);
    Serial.print("\t\t");
    Serial.print("R: ");
    Serial.println(Rval);
  }

  SonicSens();
  if(distance < 20)
  {
    Stop();
  }
  else
  {
    if(midVal >= 500 && Rval >= 500 && Lval >= 500)
    {
      Stop();
    }
    else if(midVal >= 500)
    {
      Forward(100);
    }
    else if(Rval >= 500 && Lval >= 500)
    {
      Nod();
    }
    else if(Rval >= 500)
    {
      Right(100);
    }
    else if(Lval >= 500)
    {
      Left(100);
    }
    else
    {
      Search();
    }
  }
}

//Movements
void Forward(int speed)
{
//Motors A
  digitalWrite(INA, clockWise);
  analogWrite(PWMA, speed);
//Motors B
  digitalWrite(INB, clockWise);
  analogWrite(PWMB, speed);
}

void ForwardRight(int speed)
{
// Motors A
  digitalWrite(INA, clockWise);
  analogWrite(PWMA, speed/2);
// Motors B
  digitalWrite(INB, clockWise);
  analogWrite(PWMB, speed);
}

void Right(int speed)
{
//Motors A
  digitalWrite(INA, clockWise);
  analogWrite(PWMA, speed);
// Motors B
  digitalWrite(INB, antiClockwise);
  analogWrite(PWMB, speed);
}

void ForwardLeft(int speed)
{
// Motors A
  digitalWrite(INA, clockWise);
  analogWrite(PWMA, speed);
// Motors B
  digitalWrite(INB, clockWise);
  analogWrite(PWMB, speed/2);
}

void Left(int speed)
{
//Motors A
  digitalWrite(INA, antiClockwise);
  analogWrite(PWMA, speed);
//Motors B
  digitalWrite(INB, clockWise);
  analogWrite(PWMB, speed);
}

void Backward(int speed)
{
//Motors A
  digitalWrite(INA, antiClockwise);
  analogWrite(PWMA, speed);
//Motors B
  digitalWrite(INB, antiClockwise);
  analogWrite(PWMB, speed);
}

void BackwardRight(int speed)
{
// Motors A
  digitalWrite(INA, antiClockwise);
  analogWrite(PWMA, speed/2);
// Motors B
  digitalWrite(INB, antiClockwise);
  analogWrite(PWMB, speed);
}

void BackwardLeft(int speed)
{
// Motors A
  digitalWrite(INA, antiClockwise);
  analogWrite(PWMA, speed);
// Motors B
  digitalWrite(INB, antiClockwise);
  analogWrite(PWMB, speed/2);
}

void Stop()
{
//Motors A
  digitalWrite(INA, clockWise);
  analogWrite(PWMA, 0);
//Motors B
  digitalWrite(INB, clockWise);
  analogWrite(PWMB, 0);
}
