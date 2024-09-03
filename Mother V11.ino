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
int nodeNumber = 2;
const int sensorPin = 33;
const int motor2FPin = 22;  //25 4 34
const int motor2RPin = 23;
const int motor3FPin = 19;
const int motor3RPin = 21;

const int flimitPin = 35;
const int encoder1Pin = 32;

int potValue = 0;
int sensorValue = 0;
int angleValue = 0;
int AutoangleValue = 0;
int motorValue = 5;
int pulseValue;
int movR = 0;
String a;

int motorDir = 0;
int count = 2000;
int pcount = 0;
int ccount = 0;
int bCount = 0;
int brcvCount = 0;

int positioncount = 0;

int flimitState = 0;
int encoder1State = 0;

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
  jsonReadings["mcount"] = count;
  jsonReadings["mlimit"] = flimitState;
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
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
  JSONVar myObject = JSON.parse(msg.c_str());
  int node = myObject["Node"];
  double temp = myObject["Mother"];
  double hum = myObject["Baby"];
  movR = temp;
  double pres = myObject["Lifting"];
  double countB = myObject["bcount"];
  brcvCount = countB;
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

  pinMode(flimitPin, INPUT);
  pinMode(encoder1Pin, INPUT);

  xTaskCreatePinnedToCore(Task2code,"Task2",10000,NULL,1,&Task2,1);
  delay(500);
}


//Task2code: blinks an LED every 700 ms
void Task2code( void * pvParameters )
{
  for(;;)
  {
    getDistance();
    if(movR<3000)
    {
      sensorValue = movR;
    }
    if(brcvCount<5000)
    {
      bCount = brcvCount;
    }    
    flimitState = digitalRead(flimitPin);
    if(sensorValue>0 && sensorValue<2009)
    {
      if(sensorValue == 2000)
      {
        motorDir = 1;
        if(flimitState==1)
        {
          digitalWrite(motor2FPin, HIGH);
          digitalWrite(motor2RPin, LOW);
          digitalWrite(motor3FPin, HIGH);
          digitalWrite(motor3RPin, LOW);
        }
        if(flimitState==0)
        {
          digitalWrite(motor2FPin, LOW);
          digitalWrite(motor2RPin, LOW);
          digitalWrite(motor3FPin, LOW);
          digitalWrite(motor3RPin, LOW);
          count = 2000;
        }
      }
      else if(sensorValue == 2010)
      {
        motorDir = 1;
        if(flimitState==1)
        {
          digitalWrite(motor2FPin, HIGH);
          digitalWrite(motor2RPin, LOW);
          digitalWrite(motor3FPin, HIGH);
          digitalWrite(motor3RPin, LOW);
        }
        if(flimitState==0)
        {
          digitalWrite(motor2FPin, LOW);
          digitalWrite(motor2RPin, LOW);
          digitalWrite(motor3FPin, LOW);
          digitalWrite(motor3RPin, LOW);
          count = 2001;
        }
      }
      else
      {
        if(sensorValue<count)
        {
          motorDir = 1;
          if(flimitState==1 && bCount == 1005)
          {
            digitalWrite(motor2FPin, HIGH);
            digitalWrite(motor2RPin, LOW);
            digitalWrite(motor3FPin, HIGH);
            digitalWrite(motor3RPin, LOW);
          }
          if(flimitState==0)
          {
            digitalWrite(motor2FPin, LOW);
            digitalWrite(motor2RPin, LOW);
            digitalWrite(motor3FPin, LOW);
            digitalWrite(motor3RPin, LOW);
            count = 2000;
          }
        }
        else if(sensorValue==count && sensorValue!=2010)
        {
          digitalWrite(motor2FPin, LOW);
          digitalWrite(motor2RPin, LOW);
          digitalWrite(motor3FPin, LOW);
          digitalWrite(motor3RPin, LOW);
        }
        else if(sensorValue>count && bCount == 1005)
        {
          motorDir = 3;
          digitalWrite(motor2FPin, LOW);
          digitalWrite(motor2RPin, HIGH);
          digitalWrite(motor3FPin, LOW);
          digitalWrite(motor3RPin, HIGH);
        }
      }
    }
    else
    {
      digitalWrite(motor2FPin, LOW);
      digitalWrite(motor2RPin, LOW);
      digitalWrite(motor3FPin, LOW);
      digitalWrite(motor3RPin, LOW);
    }
  }
}

void loop() {
  mesh.update();
}

void getDistance()
{
  encoder1State = digitalRead(encoder1Pin);
  ccount = encoder1State;
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
  //Serial.print("Received = ");
  //Serial.print(sensorValue);
  //Serial.print("  Pulse = ");
  //Serial.print(count);
  //Serial.println();
  pcount = ccount;
}