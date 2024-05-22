//Libraries
#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>

//I2C
#define SlaveAddress 10
#define AnswerSize 8

String answer = "Criminal";

//RFID
#define RST_PIN 3
#define SS_PIN 4
MFRC522 mfrc522(SS_PIN, RST_PIN);

byte ArtinUID[4] = {0x4A, 0x77, 0xD7, 0x36};

//LCD
const int rs = 8, en = 5, d4 = 10, d5 = 9, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//Buzzer
#define BUZZ 2
int i,d;

void setup() 
{
  //COMS
	Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.clear();
  //I2C
  Wire.begin(SlaveAddress);
  Wire.onReceive(receiveEvent);
  //RFID
	SPI.begin();
	mfrc522.PCD_Init();
	delay(4);	
	mfrc522.PCD_DumpVersionToSerial();
	while (!Serial);
}

void loop() 
{
	if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
		return;
	}

	if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
		return;
	}

  if(mfrc522.uid.uidByte[0] == ArtinUID[0] && mfrc522.uid.uidByte[1] == ArtinUID[1] && mfrc522.uid.uidByte[2] == ArtinUID[2] && mfrc522.uid.uidByte[3] == ArtinUID[3])
  {
    Artin();
  }
  else
  {
    Criminal();
  }
  mfrc522.PICC_HaltA();
}

//I2C 
void SendEvent()
{
  byte response[AnswerSize];
  for(byte i=0; i<AnswerSize; i++)
  {
    response[i] = (byte)answer.charAt(i);
  }

  Wire.write(response,sizeof(response));
  Serial.println("Send Event");
}

void receiveEvent()
{
  while(Wire.available())
  {
    byte x = Wire.read();
    Serial.println(x);
  }
  Serial.println("Receive Event");
}

//IDs
void Artin()
{
  if(mfrc522.PICC_IsNewCardPresent())
  {
  lcd.clear();
  }
  Serial.println("Artin");
  lcd.setCursor(0, 0);
  lcd.print("ID: Artin      ");
  lcd.setCursor(0, 1);
  lcd.print("Short KING     ");
}

void Criminal()
{
  byte x = Wire.read();

  //Send signal to Master Arduino to initiate POLICE CHASE
  Wire.onRequest(SendEvent);

  Serial.println("! Artin");
  lcd.setCursor(0, 0);
  lcd.print("ID: Unknown      ");
  lcd.setCursor(0, 1);
  lcd.print("Access DENIED    ");
  delay(500);
  /*
  for(d=0; d<=5; d++)
  {
    for(i=635; i<=912; i++)
    {
      tone(BUZZ, i);
      delay(10);
      if(x == 1)
      {
        break;
      }
    }
    for(i=912; i>=635; i--)
    {
      tone(BUZZ, i);
      delay(10);
      if(x == 1)
      {
        break;
      }
    }
  }
  noTone(BUZZ);
  */
}