#include <SPI.h>
#include <RF24.h>

#define relayPin1 A0
#define relayPin2 A1
#define relayPin3 A2
#define relayPin4 A3
#define relayPin5 2
#define relayPin6 3
#define relayPin7 4
#define relayPin8 5
#define relayPin9 6

int buttonState = 0;

RF24 radio(9, 10); // CE, CSN
const byte address1[10] = "00003";

void setup()
{
  Serial.begin(2000000);
  pinMode(relayPin1, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  pinMode(relayPin3, OUTPUT);
  pinMode(relayPin4, OUTPUT);
  pinMode(relayPin5, OUTPUT);
  pinMode(relayPin6, OUTPUT);
  pinMode(relayPin7, OUTPUT);
  pinMode(relayPin8, OUTPUT);
  pinMode(relayPin9, OUTPUT);

  digitalWrite(relayPin1, HIGH);
  digitalWrite(relayPin2, HIGH);
  digitalWrite(relayPin3, HIGH);
  digitalWrite(relayPin4, HIGH);
  digitalWrite(relayPin5, LOW);
  digitalWrite(relayPin6, HIGH);
  digitalWrite(relayPin7, LOW);
  digitalWrite(relayPin8, HIGH);
  digitalWrite(relayPin9, LOW);

  radio.begin();
  radio.openReadingPipe(0, address1);
  radio.setPALevel(RF24_PA_MIN);
}

void loop()
{
  radio.startListening();
  while (!radio.available());
  radio.read(&buttonState, sizeof(buttonState));
  Serial.println(buttonState);

  if (buttonState == 0) 
  {
    digitalWrite(relayPin1, HIGH);
    digitalWrite(relayPin2, HIGH);
    digitalWrite(relayPin3, HIGH);
    digitalWrite(relayPin4, HIGH);
    digitalWrite(relayPin5, LOW);
    digitalWrite(relayPin6, HIGH);
    digitalWrite(relayPin7, HIGH);
    digitalWrite(relayPin8, LOW);
    digitalWrite(relayPin9, LOW);
  }
  else  if (buttonState == 1) 
  {
    digitalWrite(relayPin1, LOW);
    digitalWrite(relayPin2, HIGH);
    digitalWrite(relayPin3, HIGH);
    digitalWrite(relayPin4, HIGH);
    digitalWrite(relayPin5, LOW);
    digitalWrite(relayPin6, HIGH);
    digitalWrite(relayPin7, LOW);
    digitalWrite(relayPin8, HIGH);
    digitalWrite(relayPin9, LOW);
  }  
  else  if (buttonState == 2) 
  {
    digitalWrite(relayPin1, HIGH);
    digitalWrite(relayPin2, HIGH);
    digitalWrite(relayPin3, HIGH);
    digitalWrite(relayPin4, HIGH);
    digitalWrite(relayPin5, LOW);
    digitalWrite(relayPin6, HIGH);
    digitalWrite(relayPin7, HIGH);
    digitalWrite(relayPin8, LOW);
    digitalWrite(relayPin9, HIGH);
  }
  else if (buttonState == 10) 
  {
    digitalWrite(relayPin1, HIGH);
    digitalWrite(relayPin2, HIGH);
    digitalWrite(relayPin3, HIGH);
    digitalWrite(relayPin4, HIGH);
    digitalWrite(relayPin5, HIGH);
    digitalWrite(relayPin6, LOW);
    digitalWrite(relayPin7, LOW);
    digitalWrite(relayPin8, HIGH);
    digitalWrite(relayPin9, LOW);
  }
  else  if (buttonState == 11) 
  {
    digitalWrite(relayPin1, HIGH);
    digitalWrite(relayPin2, LOW);
    digitalWrite(relayPin3, HIGH);
    digitalWrite(relayPin4, HIGH);
    digitalWrite(relayPin5, HIGH);
    digitalWrite(relayPin6, LOW);
    digitalWrite(relayPin7, LOW);
    digitalWrite(relayPin8, HIGH);
    digitalWrite(relayPin9, LOW);
  }
  else  if (buttonState == 12) 
  {
    digitalWrite(relayPin1, HIGH);
    digitalWrite(relayPin2, HIGH);
    digitalWrite(relayPin3, LOW);
    digitalWrite(relayPin4, HIGH);
    digitalWrite(relayPin5, HIGH);
    digitalWrite(relayPin6, LOW);
    digitalWrite(relayPin7, LOW);
    digitalWrite(relayPin8, HIGH);
    digitalWrite(relayPin9, LOW);
  }
  else  if (buttonState == 13) 
  {
    digitalWrite(relayPin1, HIGH);
    digitalWrite(relayPin2, LOW);
    digitalWrite(relayPin3, LOW);
    digitalWrite(relayPin4, HIGH);
    digitalWrite(relayPin5, HIGH);
    digitalWrite(relayPin6, LOW);
    digitalWrite(relayPin7, LOW);
    digitalWrite(relayPin8, HIGH);
    digitalWrite(relayPin9, LOW);
  }
  else  if (buttonState == 14) 
  {
    digitalWrite(relayPin1, HIGH);
    digitalWrite(relayPin2, HIGH);
    digitalWrite(relayPin3, HIGH);
    digitalWrite(relayPin4, LOW);
    digitalWrite(relayPin5, HIGH);
    digitalWrite(relayPin6, LOW);
    digitalWrite(relayPin7, LOW);
    digitalWrite(relayPin8, HIGH);
    digitalWrite(relayPin9, LOW);
  }
}
