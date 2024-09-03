#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "painlessMesh.h"
#include <Arduino_JSON.h>

// MESH Details
#define   MESH_PREFIX     "RNTMESH" //name for your MESH
#define   MESH_PASSWORD   "MESHpassword" //password for your MESH
#define   MESH_PORT       5555 //default port

//BME object on the default I2C pins
Adafruit_BME280 bme;

//Number for this node
int nodeNumber = 3;

const int potPin = 34;
const int motorlPin = 25;
const int motorrPin = 33;
const int motor2FPin = 22;
const int motor2RPin = 23;
const int encoder1Pin = 35;
const int encoder2Pin = 32;

const int sensorPin = 32;
const int motor3FPin = 18;
const int motor3RPin = 19;

int encoder1State = 0;
int encoder2State = 0;

int potValue = 0;
int sensorValue = 0;
int angleValue = 0;
int AutoangleValue = 0;
int motorValue = 5;
int pulseValue;
int movR = 0;
int movRA = 0;
int CA = 0;
String a;

int motorDir = 0;
int count = 1005;
int pcount = 0;
int ccount = 0;

//String to send to other nodes with sensor readings
String readings;

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain
String getReadings(); // Prototype for sending sensor readings

TaskHandle_t Task1;
TaskHandle_t Task2;
//Create tasks: to send messages and get readings;
Task taskSendMessage(TASK_SECOND , TASK_FOREVER, &sendMessage);

String getReadings () {
  JSONVar jsonReadings;
  jsonReadings["node"] = nodeNumber;
  jsonReadings["temp"] = 0;
  jsonReadings["hum"] = 0;
  jsonReadings["pres"] = 0;
  jsonReadings["bcount"] = count;
  jsonReadings["bAngleValue"] = CA;
  
  readings = JSON.stringify(jsonReadings);
  return readings;
}

void sendMessage () {
  String msg = getReadings();
  mesh.sendBroadcast(msg);
}

//Init BME280
void initBME(){
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }  
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  
  //Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
  
  //Serial.println();
  JSONVar myObject = JSON.parse(msg.c_str());
  int node = myObject["Node"];
  double temp = myObject["Mother"];
  double hum = myObject["Baby"];
  double pres = myObject["Lifting"];
  movR = hum;
  movRA = pres;
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
} 

void setup() {
  Serial.begin(115200);
  
  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages........18602662666

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask(taskSendMessage);
  taskSendMessage.enable();
  pinMode(sensorPin, INPUT_PULLUP);
  pinMode(motor2FPin, OUTPUT);
  pinMode(motor2RPin, OUTPUT);
  pinMode(motor3FPin, OUTPUT);
  pinMode(motor3RPin, OUTPUT);
  pinMode(motorlPin, OUTPUT);
  pinMode(motorrPin, OUTPUT);
  pinMode(encoder1Pin, INPUT);
  pinMode(encoder2Pin, INPUT);

  xTaskCreatePinnedToCore(Task2code,"Task2",10000,NULL,1,&Task2,1);
  delay(500);
}


//Task2code: blinks an LED every 700 ms
void Task2code( void * pvParameters )
{
  for(;;)
  {
    getDistance();
    
    potValue = analogRead(potPin);
    CA = (potValue-750)/25;

    if(movR<1100)
    {
      if(movRA>1049 && movRA<1100)
      {
        angleValue = movRA-1000;
      }
      else
      {
        angleValue = 0;
      }
    }

    pulseValue = 750+(angleValue*25);

    if(angleValue==0)
    {
      pulseValue = potValue;
    }
    if(potValue<4550 and potValue>750)
    {
      if(pulseValue<(potValue-60))
      {
        digitalWrite(motorlPin, HIGH);
        digitalWrite(motorrPin, LOW);
        //Serial.print("left = ");
        //Serial.print((potValue-750)/25);
      }
      else if(pulseValue>(potValue+60))
      {
        digitalWrite(motorlPin, LOW);
        digitalWrite(motorrPin, HIGH);
        //Serial.print("  Right = ");
        //Serial.print((potValue-750)/25);
      }
      else if(pulseValue>(potValue-60) and pulseValue<(potValue+60))
      {
        digitalWrite(motorlPin, LOW);
        digitalWrite(motorrPin, LOW);
        //Serial.print("  Stop = ");
        //Serial.print((potValue-750)/25);
      }
      
    }
    else
    {
      digitalWrite(motorlPin, LOW);
      digitalWrite(motorrPin, LOW);
    }
    if(movR>1000 && movR<1045)
    {
      sensorValue = movR;
    }
    
    encoder1State = digitalRead(encoder1Pin);
    encoder2State = digitalRead(encoder2Pin);
    if(sensorValue==1010)
    {
      if(encoder1State==1 && encoder2State==1)
      {
        digitalWrite(motor2FPin, LOW);
        digitalWrite(motor2RPin, LOW);
        motorDir = 0;
        count = 1005;
      }
      else
      {
        if(1005<count)
        {
          motorDir = 1;
          digitalWrite(motor2FPin, HIGH);
          digitalWrite(motor2RPin, LOW);
        }
        else if(1005>count)
        {
          motorDir = 3;
          digitalWrite(motor2FPin, LOW);
          digitalWrite(motor2RPin, HIGH);
        }
      }
    }

    else if(sensorValue>0)
    {
      if(sensorValue<count)
      {
        motorDir = 1;
        digitalWrite(motor2FPin, HIGH);
        digitalWrite(motor2RPin, LOW);
      }
      else if(sensorValue==count)
      {
        digitalWrite(motor2FPin, LOW);
        digitalWrite(motor2RPin, LOW);
        motorDir = 0;
      }
      else if(sensorValue>count)
      {
        motorDir = 3;
        digitalWrite(motor2FPin, LOW);
        digitalWrite(motor2RPin, HIGH);
      }
      
    }
    
    Serial.print("  RA = ");
    Serial.print(angleValue);
    Serial.print("  RM = ");
    Serial.print(sensorValue);
    Serial.print("  CA = ");
    Serial.print((potValue-750)/25);
    Serial.print("  Count = ");
    Serial.print(count);
    Serial.println();
  }
}

void loop() {
  mesh.update();
}

void getDistance()
{
  if(motorDir==3)
  {
    encoder1State = digitalRead(encoder1Pin);
    ccount = encoder1State;
  }
  if(motorDir==1)
  {
    encoder2State = digitalRead(encoder2Pin);
    ccount = encoder2State;
  }
  if(motorDir==3)
  {
    if (ccount!=pcount)
    {
      if(ccount==1)
      {
        count = count+1;
      }
    }
    else
    {
      count = count;
    }
  }

  if(motorDir==1)
  {
    if (ccount!=pcount)
    {
      if(ccount==1)
      {
        count = count-1;
      }
    }
    else
    {
      count = count;
    }
  }
  
  //Serial.print(count);
  pcount = ccount;
}