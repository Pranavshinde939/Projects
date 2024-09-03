#include <IBusBM.h>
#include <Servo.h>
#include <Adafruit_DPS310.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
//#include <Adafruit_NeoPixel.h>
//#define PIN       23
//#define NUMPIXELS 8

IBusBM ibus;
Adafruit_DPS310 dps;
Adafruit_Sensor *dps_temp = dps.getTemperatureSensor();
Adafruit_Sensor *dps_pressure = dps.getPressureSensor();
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

//Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

sensors_event_t event;
TaskHandle_t Task1;
TaskHandle_t Task2;

int rst = 0;
int value = 0;
int ch1 = 0;
int ch2 = 0;
int ch3 = 0;
int ch4 = 0;
int ch5 = 0;
int ch6 = 0;
int ch7 = 0;
int ch8 = 0;
int ch9 = 0;
int ch10 = 0;

int AileronPosition = 0;
int ElevatorPosition = 0;
int ThrottlePosition = 0;
int RudderPosition = 0;
int AuxPosition = 0;
int PumpSpeed = 0;

float alt = 0.00;
float a = 0.00;
float p = 0.00;
float t = 0.00;

float heading = 0;
float declinationAngle = 0.22;
float headingDegrees = 0;

static const int aileronPin = 25;
static const int elevatorPin = 33;
static const int throttlePin = 32;
static const int rudderPin = 27;
static const int auxPin = 26;

//static const int pumppwmPin = 13;
static const int pump1Pin = 12;
static const int pump2Pin = 14;

//int freq2 = 5000;
//int pwmpChannel = 0;
//int resolution2 = 8;
//const int enablePin = 13;

Servo Aileron;
Servo Elevator;
Servo Throttle;
Servo Rudder;
Servo Aux;
//Servo Pump;

int readChannel(byte channelInput, int minLimit, int maxLimit, int defaultValue)
{
  defaultValue = 0;
  uint16_t ch = ibus.readChannel(channelInput);
  if (ch < 100) return defaultValue;
  return map(ch, 1000, 2000, minLimit, maxLimit);
}

// Read the channel and return a boolean value
bool readSwitch(byte channelInput, bool defaultValue)
{
  int intDefaultValue = (defaultValue) ? 100 : 0;
  int ch = readChannel(channelInput, 0, 100, intDefaultValue);
  return (ch > 50);
}

void setup()
{
  Serial.begin(115200);
  ibus.begin(Serial2);

  Aileron.attach(aileronPin);
  Elevator.attach(elevatorPin);
  Throttle.attach(throttlePin);
  Rudder.attach(rudderPin);
  Aux.attach(auxPin);
  //Pump.attach(13);

  pinMode(13, OUTPUT);
  pinMode(pump1Pin, OUTPUT);
  pinMode(pump2Pin, OUTPUT);
  
  //pinMode(enablePin, OUTPUT);
  //ledcSetup(pwmpChannel, freq2, resolution2);
  //ledcAttachPin(enablePin, pwmChannel);

  while (!Serial) delay(10);

  Serial.println("DPS310");
  if (! dps.begin_I2C())
  {
    Serial.println("Failed to find DPS");
    while (1) yield();
  }
  Serial.println("DPS OK!");

  // Setup highest precision
  dps.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
  dps.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);

  dps_temp->printSensorDetails();
  dps_pressure->printSensorDetails();

  Serial.println("HMC5883 Magnetometer Test"); Serial.println("");
  if(!mag.begin())
  {
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
    while(1);
  }

  xTaskCreatePinnedToCore(Task1code,"Task1",10000,NULL,1,&Task1,0);          /* pin task to core 0 */
  xTaskCreatePinnedToCore(Task2code,"Task2",10000,NULL,1,&Task2,1);          /* pin task to core 1 */

  //pixels.begin(); // INITIALIZE NeoPixel strip object
  //pixels.setBrightness(150);
  //pixels.clear();
  //neopixel();
}


//Task1code:
void Task1code( void * pvParameters )
{
  for(;;)
  {
    //ch1 = 0;
    getreadings();
    getaltitude();
    getdirection();
  } 
}

//Task2code:
void Task2code( void * pvParameters )
{
  for(;;)
  {
    writereadings();
    AileronPosition   = map(ch1, 0, 4096, 0, 180);
    ElevatorPosition  = map(ch2, 0, 4096, 0, 180);
    ThrottlePosition  = map(ch3, 0, 4096, 0, 180);
    RudderPosition    = map(ch4, 0, 4096, 0, 180);
    AuxPosition       = map(ch5, 0, 4096, 0, 180);
    PumpSpeed         = map(ch6, 0, 4096, 0, 255);

    Aileron.write(AileronPosition);
    Elevator.write(ElevatorPosition);
    Throttle.write(ThrottlePosition);
    Rudder.write(RudderPosition);
    Aux.write(AuxPosition);

    operatePump();
  }
}

void getreadings()
{
  for (byte i = 0; i < 10; i++)
  {
    //readChannel;
    value = readChannel(i, 0, 4096, 0); //Serial.print("Ch"); //Serial.print(i + 1); //Serial.print(": ");
    int boolvalue = (readSwitch(i, false));
  
    if (i<10)
    {
      //ch1 = 0;
      if(i == 0)
      {
        ch1 = value;
      }
      else if(i == 1)
      {
        ch2 = value;
      }
      else if(i == 2)
      {
        ch3 = value;
      }
      else if(i == 3)
      {
        ch4 = value;
      }
      else if(i == 4)
      {
        ch5 = value;
      }
      else if(i == 5)
      {
        ch6 = value;
      }        
      else if(i == 6)
      {
        ch7 = value;
      }
      else if(i == 7)
      {
        ch8 = value;
      }
      else if(i == 8)
      {
        ch9 = value;
      }
      else if(i == 9)
      {
        ch10 = value;
      }
    }
  }
}

void operatePump()
{
  if(ch9==2048)
  {
    //Pump.write(180);
    digitalWrite(pump1Pin, HIGH);
    digitalWrite(pump2Pin, LOW);
  }
  else if(ch9==4096)
  {
    digitalWrite(13, HIGH);
    digitalWrite(pump1Pin, HIGH);
    digitalWrite(pump2Pin, LOW);
  }
  else
  {
    digitalWrite(pump1Pin, LOW);
    digitalWrite(pump2Pin, LOW);
  }
}

void neopixel()
{
  if(ch8 == 4096)
  {
    //pixels.setPixelColor(0, pixels.Color(255, 255, 255));
    //pixels.setPixelColor(1, pixels.Color(255, 255, 255));
    //pixels.setPixelColor(2, pixels.Color(255, 255, 255));
    //pixels.setPixelColor(3, pixels.Color(255, 255, 255));
    //pixels.setPixelColor(4, pixels.Color(255, 0, 0));
    //pixels.setPixelColor(5, pixels.Color(255, 0, 0));
    //pixels.setPixelColor(6, pixels.Color(255, 0, 0));
    //pixels.setPixelColor(7, pixels.Color(255, 0, 0));
  }
  else
  {
    //pixels.setPixelColor(0, pixels.Color(0, 0, 0));
    //pixels.setPixelColor(1, pixels.Color(0, 0, 0));
    //pixels.setPixelColor(2, pixels.Color(0, 0, 0));
    //pixels.setPixelColor(3, pixels.Color(0, 0, 0));
    //pixels.setPixelColor(4, pixels.Color(255, 0, 0));
    //pixels.setPixelColor(5, pixels.Color(255, 0, 0));
    //pixels.setPixelColor(6, pixels.Color(255, 0, 0));
    //pixels.setPixelColor(7, pixels.Color(255, 0, 0));
  }
  //pixels.show();
}

void getaltitude()
{
  alt = dps.readAltitude();
  if(alt>0)
  {
    a = alt;
  }
  else
  {
    a = a;
  }
}

void getdirection()
{
  mag.getEvent(&event);
  heading = atan2(event.magnetic.y, event.magnetic.x);
  declinationAngle = 0.22;
  heading += declinationAngle; 
  if(heading < 0)
    heading += 2*PI;
    
  if(heading > 2*PI)
    heading -= 2*PI;
  headingDegrees = heading * 180/M_PI;
}

void writereadings()
{
  Serial.print("Ch1");
  Serial.print(": ");
  Serial.print(ch1);
  Serial.print("  ");

  Serial.print("Ch2");
  Serial.print(": ");
  Serial.print(ch2);
  Serial.print("  ");

  Serial.print("Ch3");
  Serial.print(": ");
  Serial.print(ch3);
  Serial.print("  ");

  Serial.print("Ch4");
  Serial.print(": ");
  Serial.print(ch4);
  Serial.print("  ");

  Serial.print("Ch5");
  Serial.print(": ");
  Serial.print(ch5);
  Serial.print("  ");

  Serial.print("Ch6");
  Serial.print(": ");
  Serial.print(ch6);
  Serial.print("  ");

  Serial.print("Ch7");
  Serial.print(": ");
  Serial.print(ch7);
  Serial.print("  ");

  Serial.print("Ch8");
  Serial.print(": ");
  Serial.print(ch8);
  Serial.print("  ");

  Serial.print("Ch9");
  Serial.print(": ");
  Serial.print(ch9);
  Serial.print("  ");

  Serial.print("Ch10");
  Serial.print(": ");
  Serial.print(ch10);
  Serial.print(" | ");

  Serial.print(" Alt = ");
  Serial.print(a);
  Serial.print(" | ");

  //Serial.print("X: "); Serial.print(event.magnetic.x); Serial.print("  ");
  //Serial.print("Y: "); Serial.print(event.magnetic.y); Serial.print("  ");
  //Serial.print("Z: "); Serial.print(event.magnetic.z); Serial.print("  ");
  //Serial.print("Head (deg): "); Serial.print(headingDegrees);
  Serial.print(AileronPosition);
  Serial.print("  ");
  Serial.print(ElevatorPosition);
  Serial.print("  ");
  Serial.print(ThrottlePosition);
  Serial.print("  ");
  Serial.print(RudderPosition);
  Serial.print("  ");
  Serial.print(AuxPosition);
  Serial.print("  ");
  Serial.print(PumpSpeed);
  Serial.print(" ||");
  //Serial.print(" RST = "); Serial.print(readChannel(0, 0, 4096, 0)); Serial.print("  ");
  Serial.println();
}

void loop(){}