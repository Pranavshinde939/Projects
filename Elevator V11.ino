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
int nodeNumber = 4;
const int sensorPin = 33;
const int motor2FPin = 22;  //25 4 34
const int motor2RPin = 23;
const int motor3FPin = 19;
const int motor3RPin = 18;
const int encoder1Pin = 32;
const int encoder2Pin = 35;

const int ir1Pin = 14;
const int ir2Pin = 27;

int sensorValue = 0;
int motorValue = 5;
int movR = 0;
int movE = 0;
String a;

int motorDir = 3;
int encoder1State = 0;  // variable for reading the pushbutton status
int encoder2State = 0;
int count1 = 3000;
int pcount1 = 0;
int ccount1 = 0;
int count2 = 3000;
int pcount2 = 0;
int ccount2 = 0;
int ccountdiff = 0;
int posavg = 0;
int mdone = 0;
int mhold = 0;
int mlimit = 0;
int Mlimit = 0;

int ir1State = 0;
int ir2State = 0;

const int freq1 = 5000;
const int freq2 = 5000;
const int led1Channel = 0;
const int led2Channel = 1;
const int resolution1 = 8;
const int resolution2 = 8;
const int led1Pin = 12;
const int led2Pin = 14;

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
  jsonReadings["mdone"] = mdone;
  jsonReadings["mhold"] = mhold;
  jsonReadings["ecount"] = count1;
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
  double elev = myObject["Elevator"];
  movE = elev;
  double limit = myObject["mlimit"];
  Mlimit = limit;
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
  pinMode(encoder1Pin, INPUT);
  pinMode(encoder2Pin, INPUT);

  ledcSetup(led1Channel, freq1, resolution1);
  ledcSetup(led2Channel, freq2, resolution2);
  ledcAttachPin(26, led1Channel);
  ledcAttachPin(25, led2Channel);

  ledcWrite(led1Channel, 255);
  ledcWrite(led2Channel, 255);

  xTaskCreatePinnedToCore(Task2code,"Task2",10000,NULL,1,&Task2,1);
  delay(500);
}


//Task2code: blinks an LED every 700 ms
void Task2code( void * pvParameters )
{
  for(;;)
  {
    getDistance();
    if(Mlimit<100)
    {
      mlimit = Mlimit;
    }

    if(movE>2999 && movE<4000)
    {
      sensorValue = movE;
    }

    if(sensorValue==0)
    {
      digitalWrite(motor2FPin, LOW);
      digitalWrite(motor2RPin, LOW);
      digitalWrite(motor3FPin, LOW);
      digitalWrite(motor3RPin, LOW);
      mdone = 0;
      mhold = 1;
    }
    else
    {
      if(sensorValue>count1 && mlimit == 0)
      {
        motorDir=3;
        mdone = 0;
        mhold = 1;
        ledcWrite(led1Channel, 255);
        ledcWrite(led2Channel, 255);
        digitalWrite(motor2FPin, HIGH);
        digitalWrite(motor2RPin, LOW);
        digitalWrite(motor3FPin, HIGH);
        digitalWrite(motor3RPin, LOW);
      }
      else if(sensorValue<count1 && mlimit == 0)
      {
        motorDir=1;
        mdone = 0;
        mhold = 1;
        ledcWrite(led1Channel, 255);
        ledcWrite(led2Channel, 255);
        digitalWrite(motor2FPin, LOW);
        digitalWrite(motor2RPin, HIGH);
        digitalWrite(motor3FPin, LOW);
        digitalWrite(motor3RPin, HIGH);
      }
      else if(sensorValue==count1 )

      {
        digitalWrite(motor2FPin, LOW);
        digitalWrite(motor2RPin, LOW);
        digitalWrite(motor3FPin, LOW);
        digitalWrite(motor3RPin, LOW);
      }
      
      if(sensorValue==count1)
      {
        mdone = 1;
        mhold = 0;
      }
    }
  }
}

void loop() {
  mesh.update();
  
}

void getDistance()
{
  encoder1State = digitalRead(encoder1Pin);
  ccount1 = encoder1State;

  encoder2State = digitalRead(encoder2Pin);
  ccount2 = encoder2State;

  if(motorDir==3)
  {
    if (ccount1!=pcount1)
    {
      if(ccount1==1)
      {
        count1 = count1+1;
      }
    }
    else
    {
      count1 = count1;
    }

    if (ccount2!=pcount2)
    {
      if(ccount2==1)
      {
        count2 = count2+1;
      }
    }
    else
    {
      count2 = count2;
    }
  }

  if(motorDir==1)
  {
    if (ccount1!=pcount1)
    {
      if(ccount1==1)
      {
        count1 = count1-1;
      }
    }
    else
    {
      count1 = count1;
    }
    if (ccount2!=pcount2)
    {
      if(ccount2==1)
      {
        count2 = count2-1;
      }
    }
    else
    {
      count2 = count2;
    }
  }
  if (encoder1State == 1)
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else
  {
    digitalWrite(LED_BUILTIN, LOW);
  }
  if(count1>count2)
  {
    ccountdiff = count1-count2;
  }
  if(count1<count2)
  {
    ccountdiff = count2-count1;
  }
  if(count1==count2)
  {
    ccountdiff = 0;
  }
  Serial.print("   Incomming Motor Value = ");
  Serial.print(sensorValue);
  Serial.print("     ");
  Serial.print("Encoder 1 = ");
  Serial.print(count1);
  Serial.print("     Encoder 2 = ");
  Serial.print(count2);
  Serial.println();
  posavg = (count1+count2)/2; 
  pcount1 = ccount1;
  pcount2 = ccount2;
}