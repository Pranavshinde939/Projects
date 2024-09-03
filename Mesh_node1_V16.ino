//__ROBOT_ESP1__//

#include <Adafruit_Sensor.h>
#include "painlessMesh.h"
#include <Arduino_JSON.h>
#include <Servo.h>

// MESH Details
#define   MESH_PREFIX     "RNTMESH" //name for your MESH
#define   MESH_PASSWORD   "MESHpassword" //password for your MESH
#define   MESH_PORT       5555 //default port

//Number for this node
int nodeNumber = 1;
int node1 = 0;
int node2 = 25;
String a;

const int rmotor1Pin1 = 14;
const int rmotor1Pin2 = 27;
const int rmotor2Pin1 = 13;
const int rmotor2Pin2 = 12;

const int lmotor1Pin1 = 33;
const int lmotor1Pin2 = 32;
const int lmotor2Pin1 = 26;
const int lmotor2Pin2 = 25;

const int rEN1Pin1 = 22;
const int rEN1Pin2 = 23;
const int lEN1Pin1 = 21;
const int lEN1Pin2 = 19;

int REncoderAvg = 0;
int LEncoderAvg = 0;

int Encoder1 = 0;
int Encoder2 = 0;
int Encoder3 = 0;
int Encoder4 = 0;

int EN1 = 0;
int EN2 = 0;
int EN3 = 0;
int EN4 = 0;

int pEN1count = 0;
int pEN2count = 0;
int pEN3count = 0;
int pEN4count = 0;

int cEN1count = 0;
int cEN2count = 0;
int cEN3count = 0;
int cEN4count = 0;

int incomingTurn = 0;
int incomingDirection = 0;
int incomingSpeed = 0;
int incomingCntCmd = 0;
int incomingAutoCntCmd = 0;
int AvgCount = 0;
int FDirection = 1;
int RDirection = 0;
int CntError = 1;
int Length = 0;
int Width = 0;

const int freq1 = 5000;
const int freq2 = 5000;
const int freq3 = 5000;
const int freq4 = 5000;

const int RM1Channel = 0;
const int RM2Channel = 1;
const int LM1Channel = 2;
const int LM2Channel = 4;

const int resolution1 = 8;
const int resolution2 = 8;
const int resolution3 = 8;
const int resolution4 = 8;
const int RM1SpeedPin = 18;
const int RM2SpeedPin = 5;
const int LM1SpeedPin = 15;
const int LM2SpeedPin = 4;

int motorDir = 0;
int TmotorDir = 0;
int ccount1 = 0;
int ccount2 = 0;
int ccount3 = 0;
int ccount4 = 0;
int pcount1 = 0;
int pcount2 = 0;
int pcount3 = 0;
int pcount4 = 0;
int posavg = 0;
int count1 = 200;
int count2 = 200;
int count3 = 0;
int count4 = 0;

int cTuneLCount1 = 0;
int cTuneLCount2 = 0;
int cTuneRCount1 = 0;
int cTuneRCount2 = 0;
int pTuneLCount1 = 0;
int pTuneLCount2 = 0;
int pTuneRCount1 = 0;
int pTuneRCount2 = 0;
int TuneLCount1 = 0;
int TuneLCount2 = 0;
int TuneRCount1 = 0;
int TuneRCount2 = 0;
float TuneLCountRatio1 = 0;
float TuneLCountRatio2 = 0;
float TuneRCountRatio1 = 0;
float TuneRCountRatio2 = 0;

int Poscount1 = 200;
int Poscount2 = 200;
int posCcount1 = 200;
int posCcount2 = 200;
int posPcount1 = 200;
int posPcount2 = 200;

int rightTurnFlg = 0;
int leftTurnFlg = 0;
int cRTurn = 0;
int cLTurn = 0;
int pRTurn = 0;
int pLTurn = 0;
int rightTurnCnt = 0;
int leftTurnCnt = 0;

int cForward = 0;
int pForward = 0;
int cReverce = 0;
int pReverce = 0;

int forwardDistance = 0;
int forwardDirflg = 0;
int reverceDirflg = 0;
int forwardDirCnt = 0;
int reverceDirCnt = 0;

int operationTurnFlg = 0;
int operationDistFlg = 0;

int plannerLines = 0;
int FlushMode = 0;
int CMDAutoMode = 0;
int AutoOpDone = 0;
int TurnCntDone = 0;
int LengthDone = 0;

int numTune1 = 200;
int numTune2 = 100;
int numTune3 = 100;
int numTune4 = 100;

int Esp1Fcmd = 0;
int Esp1Tcmd = 0;
int Esp1FCntcmd = 0;
int Esp1TCntcmd = 0;
int ESP1LengthCovered = 0;

unsigned long cMillis1;
unsigned long pMillis1;

int spare = 0;
int DistError = 0;

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
  jsonReadings["ROBOT_ESP1_MotorDir"] = motorDir;
  jsonReadings["ROBOT_ESP1_forwardDirCnt"] = forwardDirCnt;
  jsonReadings["ROBOT_ESP1_rightTurnCnt"] = rightTurnCnt;
  jsonReadings["ROBOT_ESP1_leftTurnCnt"] = leftTurnCnt;
  jsonReadings["ROBOT_ESP1_LengthCovered"] = ESP1LengthCovered;
  jsonReadings["ROBOT_ESP1_Posavg"] = posavg;
  jsonReadings["ROBOT_ESP1_CMD"] = node2;

  jsonReadings["ROBOT_ESP1_TurnCntDone"] = TurnCntDone;
  jsonReadings["ROBOT_ESP1_FcmdDone"] = Esp1Fcmd;
  jsonReadings["ROBOT_ESP1_FCntcmdDone"] = Esp1FCntcmd;
  jsonReadings["ROBOT_ESP1_TCntcmdDone"] = Esp1TCntcmd;
  jsonReadings["ROBOT_ESP1_Activation"] = 1;
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
  if(elev<10000) {  node1 = elev; }

  double AutoMode = myObject["AutoMode"];
  if(AutoMode<1000)  { CMDAutoMode = AutoMode;  }

  double ESP1ForwardCMD = myObject["ESP1ForwardCMD"];
  if(ESP1ForwardCMD<1000)  { Esp1Fcmd = ESP1ForwardCMD; }

  double ESP1ForwardCntCMD = myObject["ESP1ForwardCntCMD"];
  if(ESP1ForwardCntCMD<1000)  { Esp1FCntcmd = ESP1ForwardCntCMD; }

  double ESP1TurnCntCMD = myObject["ESP1TurnCntCMD"];
  if(ESP1TurnCntCMD<1000)  { Esp1TCntcmd = ESP1TurnCntCMD; }

  double ESP1TurnCMD = myObject["ESP1TurnCMD"];
  if(ESP1TurnCMD<1000)  { Esp1Tcmd = ESP1TurnCMD; }

  double DError = myObject["ROBOT_ESP2_DistError"];
  if(DError<1000)  { DistError = DError; }
  
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
  pinMode(rmotor1Pin1, OUTPUT);
  pinMode(rmotor1Pin2, OUTPUT);
  pinMode(rmotor2Pin1, OUTPUT);
  pinMode(rmotor2Pin2, OUTPUT);

  pinMode(lmotor1Pin1, OUTPUT);
  pinMode(lmotor1Pin2, OUTPUT);
  pinMode(lmotor2Pin1, OUTPUT);
  pinMode(lmotor2Pin2, OUTPUT);

  digitalWrite(lmotor1Pin1, LOW);
  digitalWrite(lmotor1Pin2, LOW);
  digitalWrite(lmotor2Pin1, LOW);
  digitalWrite(lmotor2Pin2, LOW);

  digitalWrite(rmotor1Pin1, LOW);
  digitalWrite(rmotor1Pin2, LOW);
  digitalWrite(rmotor2Pin1, LOW);
  digitalWrite(rmotor2Pin2, LOW);

  pinMode(rEN1Pin1, INPUT);
  pinMode(rEN1Pin2, INPUT);
  pinMode(lEN1Pin1, INPUT);
  pinMode(lEN1Pin2, INPUT);

  pinMode(2, OUTPUT);

  ledcSetup(RM1Channel, freq1, resolution1);
  ledcSetup(RM2Channel, freq2, resolution2);
  ledcSetup(LM1Channel, freq3, resolution3);
  ledcSetup(LM2Channel, freq4, resolution4);
  
  ledcAttachPin(RM1SpeedPin, RM1Channel);
  ledcAttachPin(RM2SpeedPin, RM2Channel);
  ledcAttachPin(LM1SpeedPin, LM1Channel);
  ledcAttachPin(LM2SpeedPin, LM2Channel);

  ledcWrite(RM1Channel, 0);
  ledcWrite(RM2Channel, 0);
  ledcWrite(LM1Channel, 0);
  ledcWrite(LM2Channel, 0);
  
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

void loop() {
  mesh.update();
}

//Task2code: blinks an LED every 700 ms
void Task2code( void * pvParameters )
{
  for(;;)
  {
    getCMD();
    refresh();
    StartEncoder();
    operatemotor();
    //print();
    printRatio();
  }
}

void getCMD()
{
  if(node1 == 0)
  {
    incomingDirection = node1;
    incomingCntCmd = node1;
    incomingAutoCntCmd = node1;
    Length = 0;
    Width = 0;
  }
  else if(node1>4999 && node1<6000)
  {
    FlushMode = node1 - 5000;
  }

  incomingCntCmd = Esp1FCntcmd;
  incomingTurn = Esp1Tcmd;
  incomingDirection = Esp1Fcmd;
  incomingAutoCntCmd = Esp1TCntcmd;
  plannerLines = Width/20;
}

void refresh()
{
  if(FlushMode == 2)
  {
    forwardDirCnt = 0;
    rightTurnCnt = 0;
    leftTurnCnt = 0;
    Poscount1 = 200;
    Poscount2 = 200;
    ESP1LengthCovered = 200;

    count1 = 200;
    count2 = 200;
    posavg = 200;
  }
  else
  {
    forwardDirCnt = forwardDirCnt;
    rightTurnCnt = rightTurnCnt;
    leftTurnCnt = leftTurnCnt;
    Poscount1 = Poscount1;
    Poscount2 = Poscount2;
    ESP1LengthCovered = ESP1LengthCovered;

    count1 = count1;
    count2 = count2;
    posavg = posavg;
  }
}

void operatemotor()
{ 
  StartCounters();
  if(incomingCntCmd == 0)
  {
    controldirection();
  }
  else if(incomingCntCmd > 200)
  {
    controldistance();
  }

  if(incomingAutoCntCmd > 0 && incomingAutoCntCmd < 100)
  {
    spare = 1;
    controlTurn();
  }
}

void controlTurn()
{
  if(incomingTurn == 5)
  {
    ledcWrite(LM1Channel, 251);         ledcWrite(RM1Channel, 220);
    ledcWrite(LM2Channel, 255);         ledcWrite(RM2Channel, 243);
    count3 = 0;
    count4 = 0;
    TuneLCount1 = 0;
    TuneLCount2 = 0;
    TuneRCount1 = 0;
    TuneRCount2 = 0;
    TuneLCountRatio1 = TuneLCount1;
    TuneLCountRatio2 = TuneLCount2;
    TuneRCountRatio1 = TuneRCount1;
    TuneRCountRatio2 = TuneRCount2;
    incomingAutoCntCmd = 0;
    rightTurnFlg = 0;
    leftTurnFlg = 0;
    timer1();
  }
  else if(incomingTurn == 6)
  {
    ledcWrite(LM1Channel, 255);         ledcWrite(RM1Channel, 195);
    ledcWrite(LM2Channel, 227);         ledcWrite(RM2Channel, 255);
    count1 = 200;
    count2 = 200;
    incomingCntCmd = 0;
    rightTurnFlg = 1;
    leftTurnFlg = 0;
    TurnCntDone = count4;
    if(count4 == incomingAutoCntCmd)
    {
      TmotorDir = 2;
      digitalWrite(rmotor1Pin1, LOW);
      digitalWrite(rmotor1Pin2, LOW);
      digitalWrite(rmotor2Pin1, LOW);
      digitalWrite(rmotor2Pin2, LOW);

      digitalWrite(lmotor1Pin1, LOW);
      digitalWrite(lmotor1Pin2, LOW);
      digitalWrite(lmotor2Pin1, LOW);
      digitalWrite(lmotor2Pin2, LOW);
    }
    else if(incomingAutoCntCmd > count4)
    {
      TmotorDir = 3;
      digitalWrite(rmotor1Pin1, LOW);
      digitalWrite(rmotor1Pin2, HIGH);
      digitalWrite(rmotor2Pin1, LOW);
      digitalWrite(rmotor2Pin2, LOW);

      digitalWrite(lmotor1Pin1, LOW);
      digitalWrite(lmotor1Pin2, HIGH);
      digitalWrite(lmotor2Pin1, LOW);
      digitalWrite(lmotor2Pin2, HIGH);
    }
    else if(incomingAutoCntCmd < count4)
    {
      TmotorDir = 1;
      digitalWrite(rmotor1Pin1, HIGH);
      digitalWrite(rmotor1Pin2, LOW);
      digitalWrite(rmotor2Pin1, LOW);
      digitalWrite(rmotor2Pin2, LOW);

      digitalWrite(lmotor1Pin1, HIGH);
      digitalWrite(lmotor1Pin2, LOW);
      digitalWrite(lmotor2Pin1, HIGH);
      digitalWrite(lmotor2Pin2, LOW);
    }
  }
  else if(incomingTurn == 4)
  {
    ledcWrite(LM1Channel, 225);         ledcWrite(RM1Channel, 255);
    ledcWrite(LM2Channel, 255);         ledcWrite(RM2Channel, 245);
    count1 = 200;
    count2 = 200;
    incomingCntCmd = 0;
    rightTurnFlg = 0;
    leftTurnFlg = 1;
    TurnCntDone = count3;
    if(count3 == incomingAutoCntCmd)
    {
      TmotorDir = 2;
      digitalWrite(rmotor1Pin1, LOW);
      digitalWrite(rmotor1Pin2, LOW);
      digitalWrite(rmotor2Pin1, LOW);
      digitalWrite(rmotor2Pin2, LOW);

      digitalWrite(lmotor1Pin1, LOW);
      digitalWrite(lmotor1Pin2, LOW);
      digitalWrite(lmotor2Pin1, LOW);
      digitalWrite(lmotor2Pin2, LOW);
    }
    else if(incomingAutoCntCmd > count3)
    {
      TmotorDir = 3;
      digitalWrite(rmotor1Pin1, LOW);
      digitalWrite(rmotor1Pin2, HIGH);
      digitalWrite(rmotor2Pin1, LOW);
      digitalWrite(rmotor2Pin2, HIGH);

      digitalWrite(lmotor1Pin1, LOW);
      digitalWrite(lmotor1Pin2, HIGH);
      digitalWrite(lmotor2Pin1, LOW);
      digitalWrite(lmotor2Pin2, LOW);
    }
    else if(incomingAutoCntCmd < count3)
    {
      TmotorDir = 1;
      digitalWrite(rmotor1Pin1, HIGH);
      digitalWrite(rmotor1Pin2, LOW);
      digitalWrite(rmotor2Pin1, HIGH);
      digitalWrite(rmotor2Pin2, LOW);

      digitalWrite(lmotor1Pin1, HIGH);
      digitalWrite(lmotor1Pin2, LOW);
      digitalWrite(lmotor2Pin1, LOW);
      digitalWrite(lmotor2Pin2, LOW);
    }
  }
}
void timer1()
{
  cMillis1 = millis();
  if((cMillis1 - pMillis1)>=1000)
  {
    TurnCntDone = 0;
    pMillis1 = cMillis1;
  }
}

void controldistance()
{
  ledcWrite(LM1Channel, 251);         ledcWrite(RM1Channel, 220);
  ledcWrite(LM2Channel, 255);         ledcWrite(RM2Channel, 243);

  if(incomingCntCmd > posavg)
  {
    motorDir = 3;
    forwardDirflg = 1;
    reverceDirflg = 0;
  }
  else if(incomingCntCmd < posavg)
  {
    motorDir = 1;
    forwardDirflg = 0;
    reverceDirflg = 1;
  }
  else
  {
    motorDir = 2;
    forwardDirflg = 0;
    reverceDirflg = 0;
  }

  if(incomingCntCmd > (posavg - CntError) && incomingCntCmd < (posavg + CntError))
  {
    //stop
    AutoOpDone = 1;
    digitalWrite(rmotor1Pin1, LOW);
    digitalWrite(rmotor1Pin2, LOW);
    digitalWrite(rmotor2Pin1, LOW);
    digitalWrite(rmotor2Pin2, LOW);

    digitalWrite(lmotor1Pin1, LOW);
    digitalWrite(lmotor1Pin2, LOW);
    digitalWrite(lmotor2Pin1, LOW);
    digitalWrite(lmotor2Pin2, LOW);
  }
  else if(incomingCntCmd > (posavg + CntError))
  {
    //forward

    if(DistError == 1)
    {
      ledcWrite(LM1Channel, 0);         ledcWrite(RM1Channel, 0);
      ledcWrite(LM2Channel, 0);         ledcWrite(RM2Channel, 0);
      digitalWrite(2, HIGH);
    }
    else if(DistError != 1)
    {
      ledcWrite(LM1Channel, 251);         ledcWrite(RM1Channel, 220);
      ledcWrite(LM2Channel, 255);         ledcWrite(RM2Channel, 243);
      digitalWrite(2, LOW);
    }

    digitalWrite(rmotor1Pin1, LOW);
    digitalWrite(rmotor1Pin2, HIGH);
    digitalWrite(rmotor2Pin1, LOW);
    digitalWrite(rmotor2Pin2, HIGH);

    digitalWrite(lmotor1Pin1, LOW);
    digitalWrite(lmotor1Pin2, HIGH);
    digitalWrite(lmotor2Pin1, LOW);
    digitalWrite(lmotor2Pin2, HIGH);
  }
  else if(incomingCntCmd < (posavg - CntError))
  {
    //rev
    
    digitalWrite(rmotor1Pin1, HIGH);
    digitalWrite(rmotor1Pin2, LOW);
    digitalWrite(rmotor2Pin1, HIGH);
    digitalWrite(rmotor2Pin2, LOW);

    digitalWrite(lmotor1Pin1, HIGH);
    digitalWrite(lmotor1Pin2, LOW);
    digitalWrite(lmotor2Pin1, HIGH);
    digitalWrite(lmotor2Pin2, LOW);
  }
}

void controldirection()
{
  if(incomingCntCmd == 0 && incomingAutoCntCmd == 0)
  {
    ledcWrite(RM1Channel, 220);
    ledcWrite(RM2Channel, 243);
    ledcWrite(LM1Channel, 251);
    ledcWrite(LM2Channel, 255);

    motorDir = 2;
    if(incomingDirection == 5)
    {
      //stop
      digitalWrite(rmotor1Pin1, LOW);
      digitalWrite(rmotor1Pin2, LOW);
      digitalWrite(rmotor2Pin1, LOW);
      digitalWrite(rmotor2Pin2, LOW);

      digitalWrite(lmotor1Pin1, LOW);
      digitalWrite(lmotor1Pin2, LOW);
      digitalWrite(lmotor2Pin1, LOW);
      digitalWrite(lmotor2Pin2, LOW);
    }
    else if(incomingDirection == 4 && incomingTurn == 5)
    {
      //rev
      digitalWrite(rmotor1Pin1, HIGH);
      digitalWrite(rmotor1Pin2, LOW);
      digitalWrite(rmotor2Pin1, HIGH);
      digitalWrite(rmotor2Pin2, LOW);

      digitalWrite(lmotor1Pin1, HIGH);
      digitalWrite(lmotor1Pin2, LOW);
      digitalWrite(lmotor2Pin1, HIGH);
      digitalWrite(lmotor2Pin2, LOW);
    }
    else if(incomingDirection == 6 && incomingTurn == 5)
    {
      //forward
      //if(EN2<EN1)

      digitalWrite(rmotor1Pin1, LOW);
      digitalWrite(rmotor1Pin2, HIGH);
      digitalWrite(rmotor2Pin1, LOW);
      digitalWrite(rmotor2Pin2, HIGH);

      //if(EN3<EN4)
      
      digitalWrite(lmotor1Pin1, LOW);
      digitalWrite(lmotor1Pin2, HIGH);
      digitalWrite(lmotor2Pin1, LOW);
      digitalWrite(lmotor2Pin2, HIGH);
    }

    else if(incomingDirection == 6 && incomingTurn == 4)
    {
      //forward
      digitalWrite(rmotor1Pin1, LOW);
      digitalWrite(rmotor1Pin2, HIGH);
      digitalWrite(rmotor2Pin1, LOW);
      digitalWrite(rmotor2Pin2, HIGH);

      digitalWrite(lmotor1Pin1, LOW);
      digitalWrite(lmotor1Pin2, HIGH);
      digitalWrite(lmotor2Pin1, LOW);
      digitalWrite(lmotor2Pin2, LOW);
    }
    else if(incomingDirection == 6 && incomingTurn == 6)
    {
      digitalWrite(rmotor1Pin1, LOW);
      digitalWrite(rmotor1Pin2, HIGH);
      digitalWrite(rmotor2Pin1, LOW);
      digitalWrite(rmotor2Pin2, LOW);

      digitalWrite(lmotor1Pin1, LOW);
      digitalWrite(lmotor1Pin2, HIGH);
      digitalWrite(lmotor2Pin1, LOW);
      digitalWrite(lmotor2Pin2, HIGH);
    }
    else if(incomingDirection == 7 && incomingTurn == 9)
    {
      digitalWrite(rmotor1Pin1, LOW);
      digitalWrite(rmotor1Pin2, HIGH);
      digitalWrite(rmotor2Pin1, LOW);
      digitalWrite(rmotor2Pin2, HIGH);

      digitalWrite(lmotor1Pin1, HIGH);
      digitalWrite(lmotor1Pin2, LOW);
      digitalWrite(lmotor2Pin1, HIGH);
      digitalWrite(lmotor2Pin2, LOW);
    }
    else if(incomingDirection == 8 && incomingTurn == 9)
    {
      digitalWrite(rmotor1Pin1, HIGH);
      digitalWrite(rmotor1Pin2, LOW);
      digitalWrite(rmotor2Pin1, HIGH);
      digitalWrite(rmotor2Pin2, LOW);

      digitalWrite(lmotor1Pin1, LOW);
      digitalWrite(lmotor1Pin2, HIGH);
      digitalWrite(lmotor2Pin1, LOW);
      digitalWrite(lmotor2Pin2, HIGH);
    }
  }
}

void StartCounters()
{
  cRTurn = rightTurnFlg;
  cLTurn = leftTurnFlg;
  cForward = forwardDirflg;
  cReverce = reverceDirflg;

  if (cRTurn!=pRTurn)
  {
    if(cRTurn==1)
    {
      rightTurnCnt = rightTurnCnt+1;
    }
  }
  else  { rightTurnCnt = rightTurnCnt;  }

  if (cLTurn!=pLTurn)
  {
    if(cLTurn==1)
    {
      leftTurnCnt = leftTurnCnt+1;
    }
  }
  else  { leftTurnCnt = leftTurnCnt;  }
  
  //////////////////////////////////////////////////
  if (cForward!=pForward)
  {
    if(cForward==1)
    {
      forwardDirCnt = forwardDirCnt+1;
    }
  }
  else  { forwardDirCnt = forwardDirCnt;  }

  if (cReverce!=pReverce)
  {
    if(cReverce==1)
    {
      reverceDirCnt = reverceDirCnt+1;
    }
  }
  else  { reverceDirCnt = reverceDirCnt;  }


  pRTurn = cRTurn;
  pLTurn = cLTurn;
  pForward = cForward;
  pReverce = cReverce;
}

void StartEncoder()
{
  Encoder1 = digitalRead(rEN1Pin1);
  Encoder2 = digitalRead(rEN1Pin2);
  Encoder3 = digitalRead(lEN1Pin1);
  Encoder4 = digitalRead(lEN1Pin2);

  ccount1 = Encoder2;
  ccount2 = Encoder4;
  ccount3 = Encoder2;
  ccount4 = Encoder4;

  cEN1count = Encoder1;
  cEN2count = Encoder2;
  cEN3count = Encoder3;
  cEN4count = Encoder4;

  cTuneLCount1 = Encoder3;
  cTuneLCount2 = Encoder4;
  cTuneRCount1 = Encoder1;
  cTuneRCount2 = Encoder2;

  posCcount1 = Encoder2;
  posCcount2 = Encoder4;

  if (cEN1count!=pEN1count)
  {
    if(cEN1count==1)
    {
      EN1 = EN1+1;
    }
  }
  else  { EN1 = EN1;  }

  if (cEN2count!=pEN2count)
  {
    if(cEN2count==1)
    {
      EN2 = EN2+1;
    }
  }
  else  { EN2 = EN2;  }

  if (cEN3count!=pEN3count)
  {
    if(cEN3count==1)
    {
      EN3 = EN3+1;
    }
  }
  else  { EN3 = EN3;  }

  if (cEN4count!=pEN4count)
  {
    if(cEN4count==1)
    {
      EN4 = EN4+1;
    }
  }
  else  { EN4 = EN4;  }

  pEN1count = cEN1count;
  pEN2count = cEN2count;
  pEN3count = cEN3count;
  pEN4count = cEN4count;

  REncoderAvg = (EN1+EN2)/2;
  LEncoderAvg = (EN3+EN4)/2;
  AvgCount = (LEncoderAvg + REncoderAvg)/2;



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

  ////////////////////////////////////////////////////////posCount

  if(motorDir==3)
  {
    if (posCcount1!=posPcount1)
    {
      if(posCcount1==1)
      {
        Poscount1 = Poscount1+1;
      }
    }
    else
    {
      Poscount1 = Poscount1;
    }

    if (posCcount2!=posPcount2)
    {
      if(posCcount2==1)
      {
        Poscount2 = Poscount2+1;
      }
    }
    else
    {
      Poscount2 = Poscount2;
    }
  }

  if(motorDir==1)
  {
    if (posCcount1!=posPcount1)
    {
      if(posCcount1==1)
      {
        Poscount1 = Poscount1-1;
      }
    }
    else
    {
      Poscount1 = Poscount1;
    }
    if (posCcount2!=posPcount2)
    {
      if(posCcount2==1)
      {
        Poscount2 = Poscount2-1;
      }
    }
    else
    {
      Poscount2 = Poscount2;
    }
  }

  ///////////////////////////////TurnCount

  if(incomingTurn==6)
  {
    if(TmotorDir == 3)
    {
      if (ccount4!=pcount4)
      {
        if(ccount4==1)
        {
          count4 = count4+1;
        }
      }
      else
      {
        count4 = count4;
      }
    }
    else if(TmotorDir == 1)
    {
      if (ccount4!=pcount4)
      {
        if(ccount4==1)
        {
          count4 = count4-1;
        }
      }
      else
      {
        count4 = count4;
      }
    }


    if(TmotorDir == 3)
    {
      if(cTuneLCount1!=pTuneLCount1)
      {
        if(cTuneLCount1==1)
        {
          TuneLCount1 = TuneLCount1+1;
        }
      }
      else
      {
        TuneLCount1 = TuneLCount1;
      }

      if(cTuneLCount2!=pTuneLCount2)
      {
        if(cTuneLCount2==1)
        {
          TuneLCount2 = TuneLCount2+1;
        }
      }
      else
      {
        TuneLCount2 = TuneLCount2;
      }

      if(cTuneRCount1!=pTuneRCount1)
      {
        if(cTuneRCount1==1)
        {
          TuneRCount1 = TuneRCount1+1;
        }
      }
      else
      {
        TuneRCount1 = TuneRCount1;
      }

      if(cTuneRCount2!=pTuneRCount2)
      {
        if(cTuneRCount2==1)
        {
          TuneRCount2 = TuneRCount2+1;
        }
      }
      else
      {
        TuneRCount2 = TuneRCount2;
      }
    }
    else if(TmotorDir == 1)
    {
      if(cTuneLCount1!=pTuneLCount1)
      {
        if(cTuneLCount1==1)
        {
          TuneLCount1 = TuneLCount1-1;
        }
      }
      else
      {
        TuneLCount1 = TuneLCount1;
      }

      if(cTuneLCount2!=pTuneLCount2)
      {
        if(cTuneLCount2==1)
        {
          TuneLCount2 = TuneLCount2-1;
        }
      }
      else
      {
        TuneLCount2 = TuneLCount2;
      }

      if(cTuneRCount1!=pTuneRCount1)
      {
        if(cTuneRCount1==1)
        {
          TuneRCount1 = TuneRCount1-1;
        }
      }
      else
      {
        TuneRCount1 = TuneRCount1;
      }

      if(cTuneRCount2!=pTuneRCount2)
      {
        if(cTuneRCount2==1)
        {
          TuneRCount2 = TuneRCount2-1;
        }
      }
      else
      {
        TuneRCount2 = TuneRCount2;
      }
    }
    TuneLCountRatio1 = TuneLCount1;
    TuneLCountRatio2 = TuneLCount2;
    TuneRCountRatio1 = TuneRCount1;
    TuneRCountRatio2 = TuneRCount2;
  }

  if(incomingTurn==4)
  {
    if(TmotorDir == 3)
    {
      if (ccount3!=pcount3)
      {
        if(ccount3==1)
        {
          count3 = count3+1;
        }
      }
      else
      {
        count3 = count3;
      }
    }
    else if(TmotorDir == 1)
    {
      if (ccount3!=pcount3)
      {
        if(ccount3==1)
        {
          count3 = count3-1;
        }
      }
      else
      {
        count3 = count3;
      }
    }

    if(TmotorDir == 3)
    {
      if(cTuneLCount1!=pTuneLCount1)
      {
        if(cTuneLCount1==1)
        {
          TuneLCount1 = TuneLCount1+1;
        }
      }
      else
      {
        TuneLCount1 = TuneLCount1;
      }

      if(cTuneLCount2!=pTuneLCount2)
      {
        if(cTuneLCount2==1)
        {
          TuneLCount2 = TuneLCount2+1;
        }
      }
      else
      {
        TuneLCount2 = TuneLCount2;
      }

      if(cTuneRCount1!=pTuneRCount1)
      {
        if(cTuneRCount1==1)
        {
          TuneRCount1 = TuneRCount1+1;
        }
      }
      else
      {
        TuneRCount1 = TuneRCount1;
      }

      if(cTuneRCount2!=pTuneRCount2)
      {
        if(cTuneRCount2==1)
        {
          TuneRCount2 = TuneRCount2+1;
        }
      }
      else
      {
        TuneRCount2 = TuneRCount2;
      }
    }
    else if(TmotorDir == 1)
    {
      if(cTuneLCount1!=pTuneLCount1)
      {
        if(cTuneLCount1==1)
        {
          TuneLCount1 = TuneLCount1-1;
        }
      }
      else
      {
        TuneLCount1 = TuneLCount1;
      }

      if(cTuneLCount2!=pTuneLCount2)
      {
        if(cTuneLCount2==1)
        {
          TuneLCount2 = TuneLCount2-1;
        }
      }
      else
      {
        TuneLCount2 = TuneLCount2;
      }

      if(cTuneRCount1!=pTuneRCount1)
      {
        if(cTuneRCount1==1)
        {
          TuneRCount1 = TuneRCount1-1;
        }
      }
      else
      {
        TuneRCount1 = TuneRCount1;
      }

      if(cTuneRCount2!=pTuneRCount2)
      {
        if(cTuneRCount2==1)
        {
          TuneRCount2 = TuneRCount2-1;
        }
      }
      else
      {
        TuneRCount2 = TuneRCount2;
      }
    }
    TuneLCountRatio1 = TuneLCount1;
    TuneLCountRatio2 = TuneLCount2;
    TuneRCountRatio1 = TuneRCount1;
    TuneRCountRatio2 = TuneRCount2;
  }

  posPcount1 = posCcount1;
  posPcount2 = posCcount2;
  ESP1LengthCovered = (Poscount1+Poscount2)/2;

  posavg = (count1+count2)/2;
  pcount1 = ccount1;
  pcount2 = ccount2;
  //posavg = (count1+count2)/2;
  pcount3 = ccount3;
  pcount4 = ccount4;

  pTuneLCount1 = cTuneLCount1;
  pTuneLCount2 = cTuneLCount2;
  pTuneRCount1 = cTuneRCount1;
  pTuneRCount2 = cTuneRCount2;
}

void printRatio()
{
  Serial.print(" TuneLCountRatio1 = ");
  Serial.print(TuneLCountRatio1);
  Serial.print(" TuneLCountRatio2 = ");
  Serial.print(TuneLCountRatio2);
  Serial.print(" TuneRCountRatio1 = ");
  Serial.print(TuneRCountRatio1);
  Serial.print(" TuneRCountRatio2 = ");
  Serial.print(TuneRCountRatio2);
  Serial.println();
}

void print()
{
  Serial.print(" Inc.CMD= ");
  Serial.print(node1);
  Serial.print(" Inc.Turn= ");
  Serial.print(incomingTurn);
  Serial.print(" Inc.Speed= ");
  Serial.print(incomingSpeed);
  Serial.print(" Inc.Dir= ");
  Serial.print(incomingDirection);
  Serial.print(" LEn1= ");
  Serial.print(EN3);
  Serial.print(" LEn2= ");
  Serial.print(EN4);
  Serial.print(" REn1= ");
  Serial.print(EN1);
  Serial.print(" REn2= ");
  Serial.print(EN2);
  //Serial.print("   LEncoderAvg = ");
  //Serial.print(LEncoderAvg);
  //Serial.print("   REncoderAvg = ");
  //Serial.print(REncoderAvg);
  Serial.print(" Inc.CntCmd= ");
  Serial.print(incomingCntCmd);
  Serial.print(" Posavg= ");
  Serial.print(posavg);
  Serial.print(" Auto= ");
  Serial.print(incomingAutoCntCmd);
  Serial.print(" ccnt3= ");
  Serial.print(count3);
  Serial.print(" ccnt4= ");
  Serial.print(count4);

  Serial.print(" ");
  Serial.print(rightTurnCnt);
  Serial.print(" ");
  Serial.print(forwardDirCnt);
  Serial.print(" ");
  Serial.print(Length);
  Serial.print(" ");
  Serial.print(Width);
  Serial.print("    ");
  Serial.print(Esp1Fcmd);
  Serial.print("  ");
  Serial.print(Esp1FCntcmd);
  Serial.print("  ");
  Serial.print(Esp1TCntcmd);
  Serial.print("  ");
  Serial.print(Esp1Tcmd);
  Serial.print("  ");
  Serial.print(ESP1LengthCovered);
  Serial.print("  ");
  Serial.print(posavg);
  Serial.print("  ");
  Serial.print(FlushMode);
  
  Serial.println();
}