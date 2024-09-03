//_ROBOT_ESP2_//


#include <Adafruit_Sensor.h>
#include "painlessMesh.h"
#include <Arduino_JSON.h>
#include <Servo.h>
//#include <Adafruit_NeoPixel.h>
#include <Simple_HCSR04.h>

// MESH Details
#define   MESH_PREFIX     "RNTMESH" //name for your MESH
#define   MESH_PASSWORD   "MESHpassword" //password for your MESH
#define   MESH_PORT       5555 //default port

//Adafruit_NeoPixel pixels(1, 21, NEO_GRB + NEO_KHZ800);
Simple_HCSR04 *sensor;

const int ECHO_PIN = 18;
const int TRIG_PIN = 19;

int distError = 0;
int distance = 0;

//Number for this node
int nodeNumber = 2;
int node1 = 0;
String a;

int count1 = 3000;

const int cutterENPin1 = 4;
//const int seederENPin1 = 2;

const int fservoPin1 = 27;
const int cservoPin1 = 14;
const int lservoPin1 = 25;
const int lservoPin2 = 26;
const int rservoPin1 = 32;
const int rservoPin2 = 33;

//const int cutterservoPin1 = 15;
//const int seederservoPin1 = 2;

int buttonState = 0;
int cutterENState = 0;
int seederENState = 0;

int fposDegrees1 = 90;
int cposDegrees1 = 90;
int lposDegrees1 = 90;
int lposDegrees2 = 90;
int rposDegrees1 = 90;
int rposDegrees2 = 90;

//int cutterposDegrees1 = 0;
//int seederposDegrees1 = 0;

int incomingTurn;
int incomingDirection;
int incomingSpeed;
int incomingCntCmd;
int MotorDir;
int cTurn;
int pTurn;

int Esp2Tcmd = 0;
int Esp2TDcmd = 0;

int TurnDone = 0;
int TurnCount = 0;

unsigned long cMillis;
unsigned long pMillis;

const int seederENPin1 = 5;

int ESP1MotorDir = 0;
int ESPConnection = 0;

Servo fservo1;
Servo cservo1;
Servo lservo1;
Servo lservo2;
Servo rservo1;
Servo rservo2;

//Servo cutterservo1;
//Servo seederservo1;

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
  jsonReadings["node"]                  = nodeNumber;
  jsonReadings["ROBOT_ESP2_TurnDone"]   = TurnDone;
  jsonReadings["ROBOT_ESP2_Activation"] = 1;
  jsonReadings["ROBOT_ESP2_DistError"]  = distError;
  readings = JSON.stringify(jsonReadings);
  return readings;
}

void sendMessage () {
  String msg = getReadings();
  mesh.sendBroadcast(msg);
}

//Init BME280

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
  JSONVar myObject = JSON.parse(msg.c_str());

  int node = myObject["Node"];

  double elev = myObject["CMD"];
  if(elev<1000) { node1 = elev; }

  double ESP2TurnCMD = myObject["ESP2TurnCMD"];
  if(ESP2TurnCMD<1000)  { Esp2Tcmd = ESP2TurnCMD; }

  double ESP2TurnCntCMD = myObject["ESP2TurnCntCMD"];
  if(ESP2TurnCntCMD<1000)  { Esp2TDcmd = ESP2TurnCntCMD; }

  double ESP1MDir = myObject["ROBOT_ESP1_MotorDir"];
  if(ESP1MDir<1000)  { ESP1MotorDir = ESP1MDir; }

  double ESPConn = myObject["ESPConnection"];
  if(ESPConn<1000)  { ESPConnection = ESPConn; }
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
  fservo1.attach(fservoPin1);
  cservo1.attach(cservoPin1);
  lservo1.attach(lservoPin1);
  lservo2.attach(lservoPin2);
  rservo1.attach(rservoPin1);
  rservo2.attach(rservoPin2);

  pinMode(cutterENPin1, OUTPUT);
  pinMode(seederENPin1, OUTPUT);
  pinMode(2, OUTPUT);

  //pixels.begin();
  //pixels.setBrightness(100);
  //pixels.setPixelColor(0, pixels.Color(255, 0, 0));
  //pixels.show();

  sensor = new Simple_HCSR04(ECHO_PIN, TRIG_PIN);

  //ledcSetup(seederChannel, freq1, resolution1);
  //ledcAttachPin(seederENPin1, seederChannel);
  //ledcWrite(seederChannel, 0);
  
  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages........18602662666

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask(taskSendMessage);
  taskSendMessage.enable();

  xTaskCreatePinnedToCore(Task2code,"Task2",10000,NULL,1,&Task2,1);
  delay(100);
}


//Task2code: blinks an LED every 700 ms
void Task2code( void * pvParameters )
{
  for(;;)
  {
    dst();
    getCMD();
    operatemotor();
    print();
    NWS();
  }
}

void loop() {
  mesh.update();
}

void NWS()
{
  if(ESPConnection == 0)
  {
    //pixels.clear();
    //pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    //pixels.show();
  }
  else if(ESPConnection == 1)
  {
    //pixels.clear();
    //pixels.setPixelColor(0, pixels.Color(0, 255, 0));
    //pixels.show();
  }
}

void getCMD()
{
  if(node1 == 0)
  {
    incomingSpeed = 1;
    incomingTurn = 5;
  }
  else
  {
    incomingTurn = Esp2Tcmd;
    incomingSpeed = Esp2TDcmd;
  }
}

void operatemotor()
{
  controlturn();
  controlapplication();
}

void controlturn()
{
  if(incomingSpeed == 0)
  {
    incomingTurn = 0;
    fposDegrees1 = 45; cposDegrees1 = 45;
    
    lposDegrees1 = 88;  rposDegrees1 = 98;
    lposDegrees2 = 87;  rposDegrees2 = 89;
    
    lservo1.write(lposDegrees1);
    lservo2.write(lposDegrees2);
    rservo1.write(rposDegrees1);
    rservo2.write(rposDegrees2);
    fservo1.write(fposDegrees1);
    cservo1.write(cposDegrees1);
  }
  else if(incomingTurn == 5)
  {
    timer5();
    lposDegrees1 = 85;  rposDegrees1 = 96;
    lposDegrees2 = 87;  rposDegrees2 = 87;
    
    lservo1.write(lposDegrees1);
    lservo2.write(lposDegrees2);
    rservo1.write(rposDegrees1);
    rservo2.write(rposDegrees2);
  }
  else if(incomingTurn == 4)
  {
    timer4();
    lposDegrees1 = 25;  rposDegrees1 = 35;
    lposDegrees2 = 90;  rposDegrees2 = 90;
    
    lservo1.write(lposDegrees1);
    lservo2.write(lposDegrees2);
    rservo1.write(rposDegrees1);
    rservo2.write(rposDegrees2);
  }
  else if(incomingTurn == 6)
  {
    timer6();
    lposDegrees1 = 130; rposDegrees1 = 155;
    lposDegrees2 = 90;  rposDegrees2 = 90;
    
    lservo1.write(lposDegrees1);
    lservo2.write(lposDegrees2);
    rservo1.write(rposDegrees1);
    rservo2.write(rposDegrees2);
  }
  else if(incomingTurn == 9)
  {
    lposDegrees1 = 130; rposDegrees1 = 35;
    lposDegrees2 = 35;  rposDegrees2 = 130;
    
    lservo1.write(lposDegrees1);
    lservo2.write(lposDegrees2);
    rservo1.write(rposDegrees1);
    rservo2.write(rposDegrees2);
  }
}

void controlapplication()
{
  if(incomingSpeed == 1)
  {
    fposDegrees1 = 45;
    cposDegrees1 = 45;
    fservo1.write(fposDegrees1);
    cservo1.write(cposDegrees1);
    digitalWrite(cutterENPin1, LOW);
    digitalWrite(seederENPin1, LOW);
    //ledcWrite(seederChannel, 0);
  }
  else if(incomingSpeed == 2)
  {
    cposDegrees1 = 45;
    cservo1.write(cposDegrees1);
    digitalWrite(seederENPin1, LOW);
    //ledcWrite(seederChannel, 0);
  }
  else if(incomingSpeed == 3)
  {
    cposDegrees1 = 90;
    cservo1.write(cposDegrees1);
    if(ESP1MotorDir == 3)
    {
      digitalWrite(seederENPin1, HIGH);
    }
    else if(ESP1MotorDir != 3)
    {
      digitalWrite(seederENPin1, LOW);
    }
    //ledcWrite(seederChannel, 255);
  }
  else if(incomingSpeed == 4)
  {
    fposDegrees1 = 45;
    fservo1.write(fposDegrees1);
    digitalWrite(cutterENPin1, LOW);
  }
  else if(incomingSpeed == 5)
  {
    fposDegrees1 = 85;
    fservo1.write(fposDegrees1);
    digitalWrite(cutterENPin1, HIGH);
  }
}

void dst()
{
  distance = sensor->measure()->cm();
  if(distance>0 && distance<1500)
  {
    if(distance>0 && distance<60)
    {
      distError = 1;
      digitalWrite(2, HIGH);
    }
    else if(distance>0 && distance>60)
    {
      distError = 0;
      digitalWrite(2, LOW);
    }
  }
  else
  {
    distError = 0;
  }
}

void print()
{
  Serial.print("   Incomming CMD = ");
  Serial.print(node1);
  Serial.print("   Incomming Turn = ");
  Serial.print(incomingTurn);
  Serial.print("   Incomming Speed = ");
  Serial.print(incomingSpeed);
  Serial.print("   DistanceError = ");
  Serial.print(distError);
  Serial.print("   Distance = ");
  Serial.print(distance);
  
  Serial.println();
}

void timer4()
{
  cMillis = millis();
  if((cMillis - pMillis)>=1000)
  {
    TurnDone = 4;
    pMillis = cMillis;
  }
}
void timer5()
{
  cMillis = millis();
  if((cMillis - pMillis)>=1000)
  {
    TurnDone = 5;
    pMillis = cMillis;
  }
}
void timer6()
{
  cMillis = millis();
  if((cMillis - pMillis)>=1000)
  {
    TurnDone = 6;
    pMillis = cMillis;
  }
}