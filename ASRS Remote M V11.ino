#include "painlessMesh.h"
#include <Arduino_JSON.h>
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel pixels(16, 13, NEO_GRB + NEO_KHZ800);

// MESH Details
#define   MESH_PREFIX     "RNTMESH" //name for your MESH
#define   MESH_PASSWORD   "MESHpassword" //password for your MESH
#define   MESH_PORT       5555 //default port

//Number for this node
int nodeNumber = 1;
const int selectPin = 23;
int selectValue = 0;
int strint;
int movT = 0;
int movR = 0;
int movBA = 0;
int movB = 0;
int movM = 0;
int movE = 0;
int mdone = 0;
int mhold = 0;
int mlimit = 0;
int Mlimit = 0;
String a;

int mLoop = 0;
int eLoop = 0;

int mrcvCount = 0;
int mCount = 0;
int brcvCount = 0;
int bCount = 0;
int ercvCount = 0;
int eCount = 0;
int bAngleValue = 0;
int AngleValue = 0;
int mainHoming = 0;
int mainHomingVar = 0;
int HomingCmdBit = 0;

int motherloopVar = 0;
int motherloopCmdBit = 0;

int storageDone = 0;
int storageDoneVar = 0;
int storageCmdBit = 0;

int AstorageDone = 0;
int AstorageDoneVar = 0;
int AstorageCmdBit = 0;

int retrivalDone = 0;
int retrivalDoneVar = 0;
int retrivalCmdBit = 0;

int R = 0;
int G = 0;
int B = 0;

int n[4] = {0, 0, 0, 0};
int n1[4] = {1, 1002, 2001, 3182};
int n2[4] = {2, 1007, 2001, 3182};

//String to send to other nodes with readings
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
  jsonReadings["Node"] = nodeNumber;
  jsonReadings["Mother"] = movM;
  jsonReadings["Baby"] = movB;
  jsonReadings["Lifting"] = movBA;
  jsonReadings["Elevator"] = movE;
  jsonReadings["mainHoming"] = mainHoming;
  jsonReadings["storageDone"] = storageDone;
  readings = JSON.stringify(jsonReadings);
  return readings;
}

void sendMessage () {
  String msg = getReadings();
  mesh.sendBroadcast(msg);
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg )
{
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
  JSONVar myObject = JSON.parse(msg.c_str());
  int node = myObject["node"];
  double temp = myObject["temp"];
  double hum = myObject["hum"];
  movR = hum;
  double pres = myObject["pres"];
  double mdone = myObject["mdone"];
  mdone = mdone;
  double mhold = myObject["mhold"];
  mhold = mhold;

  double countM = myObject["mcount"];
  mrcvCount = countM;
  double countB = myObject["bcount"];
  brcvCount = countB;
  double countE = myObject["ecount"];
  ercvCount = countE;
  double BA = myObject["bAngleValue"];
  AngleValue = BA;
  double limit = myObject["mlimit"];
  Mlimit = limit;
  
}

void newConnectionCallback(uint32_t nodeId)
{
  Serial.printf("New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback()
{
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset)
{
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void setup()
{
  Serial.begin(115200);
  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  userScheduler.addTask(taskSendMessage);
  taskSendMessage.enable();
  pinMode(selectPin, INPUT_PULLUP);
  xTaskCreatePinnedToCore(Task2code,"Task2",10000,NULL,1,&Task2,1);
  pixels.begin();
  pixels.setBrightness(255);
  delay(500);
}

void Task2code( void * pvParameters )
{
  for(;;)
  {
    palletselect();
    if(Mlimit<100)
    {
      mlimit = Mlimit;
    }

    if(mrcvCount<5000)
    {
      mCount = mrcvCount;
    }
    if(brcvCount<5000)
    {
      bCount = brcvCount;
    }
    if(ercvCount<5000)
    {
      eCount = ercvCount;
    }
    if(AngleValue<5000)
    {
      bAngleValue = AngleValue;
    }
    
    if(motherloopCmdBit == 1)
    {
      motherloop();
      Serial.print("  Mother = ");
      Serial.print(mCount);
      Serial.print("  Baby = ");
      Serial.print(bCount);
      Serial.print("  BabyAngle = ");
      Serial.print(bAngleValue);
      Serial.print("  Elevator = ");
      Serial.print(eCount);
      Serial.print("  Mode = ");
      Serial.print("  Loop  ");
      Serial.print("  E Loop = ");
      Serial.print(eLoop);
      Serial.print("  M Loop = ");
      Serial.print(mLoop);
      Serial.println();
      pixels.clear();
      pixels.setPixelColor(0, pixels.Color(0, 255, 0));
      pixels.show();
    }
    
    if(HomingCmdBit == 1)
    {
      mainhoming();
      Serial.print("  Mother = ");
      Serial.print(mCount);
      Serial.print("  Baby = ");
      Serial.print(bCount);
      Serial.print("  BabyAngle = ");
      Serial.print(bAngleValue);
      Serial.print("  Elevator = ");
      Serial.print(eCount);
      Serial.print("  Mode = ");
      Serial.print("  Homing  ");
      Serial.print(mainHoming);
      Serial.print("  Pallet select =  ");
      Serial.print(n[0]);
      Serial.println();
      pixels.clear();
      pixels.setPixelColor(0, pixels.Color(0, 255, 0));
      pixels.show();
    }
    
    if(storageCmdBit == 1)
    {
      storage();
      Serial.print("  Mother = ");
      Serial.print(mCount);
      Serial.print("  Baby = ");
      Serial.print(bCount);
      Serial.print("  BabyAngle = ");
      Serial.print(bAngleValue);
      Serial.print("  Elevator = ");
      Serial.print(eCount);
      Serial.print("  Mode = ");
      Serial.print("  Storage  ");
      Serial.print(storageDone);
      Serial.println();
      pixels.clear();
      pixels.setPixelColor(1, pixels.Color(255, 0, 0));
      pixels.show();
    }
    if(AstorageCmdBit == 1)
    {
      Serial.print("  Mother = ");
      Serial.print(mCount);
      Serial.print("  Baby = ");
      Serial.print(bCount);
      Serial.print("  BabyAngle = ");
      Serial.print(bAngleValue);
      Serial.print("  Elevator = ");
      Serial.print(eCount);
      Serial.print("  Mode = ");
      Serial.print("  Homing  ");
      Serial.print(mainHoming);
      Serial.println();
      pixels.clear();
      pixels.setPixelColor(0, pixels.Color(0, 255, 0));
      pixels.show();
    }

    if(retrivalCmdBit == 1)
    {
      retrival();
      Serial.print("  Mother = ");
      Serial.print(mCount);
      Serial.print("  Baby = ");
      Serial.print(bCount);
      Serial.print("  BabyAngle = ");
      Serial.print(bAngleValue);
      Serial.print("  Elevator = ");
      Serial.print(eCount);
      Serial.print("  Mode = ");
      Serial.print("  Retrival  ");
      Serial.print(retrivalDone);
      Serial.println();
      pixels.clear();
      pixels.setPixelColor(2, pixels.Color(0, 0, 255));
      pixels.show();
    }

    if(Serial.available( ) > 0)
    { 
      a = Serial.readString( );
      strint = a.toInt();

      if(strint == 7000)
      {
        motherloopCmdBit = 1;
        storageDone = 0;
        storageCmdBit = 0;
        AstorageCmdBit = 0;
        HomingCmdBit = 0;
        retrivalCmdBit = 0;
      }
      else if(strint == 6000)
      {
        storageDone = 0;
        storageCmdBit = 0;
        AstorageCmdBit = 0;
        HomingCmdBit = 1;
        retrivalCmdBit = 0;
        motherloopCmdBit = 0;
      }

      else if(strint == 6001)
      {
        mainHoming = 0;
        storageCmdBit = 1;
        AstorageCmdBit = 0;
        HomingCmdBit = 0;
        retrivalCmdBit = 0;
        motherloopCmdBit = 0;
      }

      else if(strint == 6002)
      {
        mainHoming = 0;
        storageCmdBit = 0;
        AstorageCmdBit = 0;
        HomingCmdBit = 0;
        retrivalCmdBit = 1;
        motherloopCmdBit = 0;
      }

      else
      {
        storageCmdBit = 0;
        AstorageCmdBit = 0;
        HomingCmdBit = 0;
        retrivalCmdBit = 0;
        motherloopCmdBit = 0;
        if(strint>1999 && strint<3000)
        {
          movM = strint;
        }
        if(strint>1000 && strint<1200)
        {
          if(strint>1000 && strint<1011)
          {
            movB = strint;
          }
          if(strint>1049 && strint<1100)
          {
            //if(strint == 1050)
            //{
            //  movBA = 1090;
            //}
            //else if(strint == 1090)
            //{
            //  movBA = 1050;
            //}
            movBA = strint;
          }
        }
        if(strint>2999 && strint<4000)
        {
          movE = strint;
        }
        if(strint==0)
        {
          movE = strint;
        }

        Serial.print("  Mother = ");
        Serial.print(mCount);
        Serial.print("  Baby = ");
        Serial.print("  BabyAngle = ");
        Serial.print(bAngleValue);
        Serial.print(bCount);
        Serial.print("  Elevator = ");
        Serial.print(eCount);
        Serial.print("  Mode = ");
        Serial.print("  Normal  ");
        Serial.println();
        pixels.clear();
        pixels.setPixelColor(3, pixels.Color(255, 50, 0));
        pixels.show();
      }
       
    }
    
    //Serial.print("  MovM = ");
    //Serial.print(movM);
    //Serial.print("  MovB = ");
    //Serial.print(movB);
    //Serial.print("  MovBA = ");
    //Serial.print(movBA);
    //Serial.print("  MovE = ");
    //Serial.print(movE);
    
  }
}

void loop() {
  mesh.update();
}

void mainhoming()
{
  if(bCount == 1005)
  {
    if(mCount == 2000)
    {
      if(eCount == 3000)
      {
        //movBA = 1050;
        mainHoming = 1;
        
      }
      else
      {
        movE = 3000;
        mainHoming = 0;
      }
    }
    else
    {
      movM = 2000;
      mainHoming = 0;
    }
  }
  else
  {
    movB = 1010;
    mainHoming = 0;
  }  
}

void storage()
{
  retrivalDone = 0;
  if(storageDone == 1)
  {
    if(bCount == 1005)
    {
      if(mCount == 2000)
      {
        if(eCount == 3000)
        {
          //movBA = 1050;
          mainHoming = 1;
          
        }
        else
        {
          movE = 3000;
          mainHoming = 0;
        }
      }
      else
      {
        movM = 2000;
        mainHoming = 0;
      }
    }
    else
    {
      movB = 1010;
      mainHoming = 0;
    }
  }
  else
  {
    if(eCount == n[3])
    {
      if(mCount == n[2])
      {
        if(bCount == n[1])
        {
          if(bAngleValue>=88)
          {
            storageDone = 1;
          }
          else
          {
            movBA = 1090;
            storageDone = 0;
          }
        }
        else
        {
          movB = n[1];
          storageDone = 0;
        }
      }
      else
      {
        movM = n[2];
        storageDone = 0;
      }
    }
    else
    {
      movE = n[3];
      storageDone = 0;
    }
  }
}

void retrival()
{
  if(retrivalDone == 1)
  {
    if(bCount == 1005)
    {
      if(mCount == 2000)
      {
        if(eCount == 3000)
        {
          //movBA = 1050;
          mainHoming = 1;
          
        }
        else
        {
          movE = 3000;
          mainHoming = 0;
        }
      }
      else
      {
        movM = 2000;
        mainHoming = 0;
      }
    }
    else
    {
      movB = 1010;
      mainHoming = 0;
    }
  }
  else
  {
    if(eCount == n[3])
    {
      if(mCount == n[2])
      {
        if(bCount == n[1])
        {
          if(bAngleValue<=51)
          {
            retrivalDone = 1;
          }
          else
          {
            movBA = 1050;
            retrivalDone = 0;
          }
        }
        else
        {
          movB = n[1];
          retrivalDone = 0;
        }
      }
      else
      {
        movM = n[2];
        retrivalDone = 0;
      }
    }
    else
    {
      movE = n[3];
      retrivalDone = 0;
    }
  }
}

void palletselect()
{
  selectValue = digitalRead(selectPin);
  if(selectValue == 1)
  {
    n[0] = n1[0];
    n[1] = n1[1];
    n[2] = n1[2];
    n[3] = n1[3];
  }
  else if(selectValue == 0)
  {
    n[0] = n2[0];
    n[1] = n2[1];
    n[2] = n2[2];
    n[3] = n2[3];
  }
}

void motherloop()
{
  if(eLoop == 0)
  {
    if(mLoop == 0)
    {
      if(mCount == 2000)
      {
        movM = 2001;
      }
      else if(mCount == 2001)
      {
        movM = 2000;
        mLoop = 1;
      }
    }
    if(mLoop == 1)
    {
      if(mCount == 2000)
      {
        mLoop = 2;
      }
    }
    if(mLoop == 2)
    {
      eLoop = 1;
      mLoop = 0;
    }
  }

  if(mLoop == 0 && eLoop == 1)
  {
    movE = 3182;
    if(eCount == 3182)
    {
      eLoop = 2;
    }
  }

  if(eLoop == 2)
  {
    if(mLoop == 0)
    {
      if(mCount == 2000)
      {
        movM = 2001;
      }
      else if(mCount == 2001)
      {
        mLoop = 1;
      }
    }
    else if(mLoop == 1)
    {
      if(mCount == 2001)
      {
        movM = 2000;
      }
      else if(mCount == 2000)
      {
        mLoop = 2;
      }
    }
    else if(mLoop == 2)
    {
      eLoop = 3;
      mLoop = 0;
    }
  }
  if(eLoop == 3)
  {
    movE = 3000;
    if(eCount == 3000)
    {
      eLoop = 0;
    }
  }
}