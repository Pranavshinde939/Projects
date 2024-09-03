#include <SPI.h>
#include <RF24.h>

#define buttonPin1 3
#define buttonPin2 4
#define buttonPin3 A5
#define buttonPin4 A4
#define buttonPin5 A3
#define buttonPin6 A2
#define buttonPin7 A1
#define buttonPin8 A0

int buttonState0 = 0;
int buttonState1 = 0;
int buttonState2 = 0;
int buttonState3 = 0;
int buttonState4 = 0;
int buttonState5 = 0;
int buttonState6 = 0;
int buttonState7 = 0;
int buttonState8 = 0;

RF24 radio(9, 10); // CE, CSN

const byte address[10] = "00003";

void setup()
{
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(buttonPin3, INPUT_PULLUP);
  pinMode(buttonPin4, INPUT_PULLUP);
  pinMode(buttonPin5, INPUT_PULLUP);
  pinMode(buttonPin6, INPUT_PULLUP);
  pinMode(buttonPin7, INPUT_PULLUP);
  pinMode(buttonPin8, INPUT_PULLUP);

  Serial.begin(1000000);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}

void loop()
{
  buttonState1 = digitalRead(buttonPin1);
  buttonState2 = digitalRead(buttonPin2);
  buttonState3 = digitalRead(buttonPin3);
  buttonState4 = digitalRead(buttonPin4);
  buttonState5 = digitalRead(buttonPin5);
  buttonState6 = digitalRead(buttonPin6);
  buttonState7 = digitalRead(buttonPin7);
  buttonState8 = digitalRead(buttonPin8);
  if (buttonState1 == 1)
  {
    if (buttonState2 == 1)
    {
      if (buttonState3 == 0 && buttonState4 == 0 && buttonState5 == 0 && buttonState6 == 0)
      {
        buttonState0 = 10;
      }
      else if (buttonState3 == 1 && buttonState4 == 0 && buttonState5 == 0 && buttonState6 == 0)
      {
        buttonState0 = 11;
      }
      else if (buttonState3 == 0 && buttonState4 == 1&&buttonState5 == 0&&buttonState6 == 0)
      {
        buttonState0 = 12;
      }
      else if (buttonState3 == 0 && buttonState4 == 0 && buttonState5 == 1 && buttonState6 == 0)
      {
        buttonState0 = 13;
      }
      else if (buttonState3 == 0 && buttonState4 == 0 && buttonState5 == 0 && buttonState6 == 1)
      {
        buttonState0 = 14;
      }
      else
      {
        buttonState0 = 2;
      }
    }
    else  if (buttonState2 == 0)
    {
      buttonState0 = 1;
    }
  }
  else  if (buttonState1 == 0)
  {
    buttonState0 = 0;
  }

  Serial.print(buttonState1);
  Serial.print(buttonState2);
  Serial.print(buttonState3);
  Serial.print(buttonState4);
  Serial.print(buttonState5);
  Serial.print(buttonState6);
  Serial.print("\t");
  Serial.println(buttonState0);
  radio.write(&buttonState0, sizeof(buttonState0));
}
