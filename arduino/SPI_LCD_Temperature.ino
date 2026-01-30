/*
  LiquidCrystal Library - Custom Characters
 
 Demonstrates how to add custom characters on an LCD  display.  
 The LiquidCrystal library works with all LCD displays that are 
 compatible with the  Hitachi HD44780 driver. There are many of 
 them out there, and you can usually tell them by the 16-pin interface.
 
 This sketch prints "I <heart> Arduino!" and a little dancing man
 to the LCD.
 
  The circuit:
 * LCD RS pin to digital pin      8
 * LCD Enable pin to digital pin   7
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * 10K potentiometer:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 * 10K poterntiometer on pin A0
 
 created21 Mar 2011
 by Tom Igoe
 Based on Adafruit's example at
 https://github.com/adafruit/SPI_VFD/blob/master/examples/createChar/createChar.pde
 
 This example code is in the public domain.
 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 
 Also useful:
 http://icontexto.com/charactercreator/
 
 */
 
 // SPI Slave:
// https://forum.arduino.cc/index.php?topic=52111.0

// 1-Wire & Temp sensor:
// https://create.arduino.cc/projecthub/TheGadgetBoy/ds18b20-digital-temperature-sensor-and-arduino-9cc806


#include <LiquidCrystal.h>
#include <OneWire.h> 
#include <DallasTemperature.h>
#include "pins_arduino.h"

#include "saturate.h"

// SPI configurations stuff
#define _SPIBuffSize_ 8
#define _SPINBytesPerData_ 4
#define _SPIMinFrameTime_ 50 // There should be at least 50ms between frames


// TEMPERATURE SENSOR STUFF
/********************************************************************/
// Data wire is plugged into pin 2 on the Arduino 
//#define ONE_WIRE_BUS 6 
#define ONE_WIRE_BUS 8 
/********************************************************************/
// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire(ONE_WIRE_BUS); 
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
/********************************************************************/ 

// byte and long buffers to receive SPI data
byte buf [_SPIBuffSize_ * _SPINBytesPerData_];
long intBuff [_SPIBuffSize_]; // long: 4 bytes

volatile byte pos;
volatile boolean process_it;
unsigned long milliseconds;

// initialize the LCD library with the numbers of the interface pins
//LiquidCrystal lcd(8, 7, 5, 4, 3, 2);
LiquidCrystal lcd(2,3, 4,5,6,7);
int lineLength = 20;

float phase0 = 0.0;
float phase1 = 0.0;
float phase2 = 0.0;
float phase3 = 0.0;
float h = 1.0/59.0;

// Value received via SPI
long distance;
float speed, totalRowingTime, numStrokesPerMin, avStrokePower, energyRowerRealkJoule;
int axisMotTemp;
int axisLoadTemp;
float kJoules;

// make some custom characters:
byte heart[8] = {
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000
};

byte smiley[8] = {
  0b00000,
  0b00000,
  0b01010,
  0b00000,
  0b00000,
  0b10001,
  0b01110,
  0b00000
};

byte frownie[8] = {
  0b00000,
  0b00000,
  0b01010,
  0b00000,
  0b00000,
  0b00000,
  0b01110,
  0b10001
};

byte armsDown[8] = {
  0b00100,
  0b01010,
  0b00100,
  0b00100,
  0b01110,
  0b10101,
  0b00100,
  0b01010
};

byte armsUp[8] = {
  0b00100,
  0b01010,
  0b00100,
  0b10101,
  0b01110,
  0b00100,
  0b00100,
  0b01010
};

byte bar31[8] = {
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
};

byte bar32[8] = {
  0b10100,
  0b10100,
  0b10100,
  0b10100,
  0b10100,
  0b10100,
  0b10100,
  0b10100
};

byte bar33[8] = {
  0b10101,
  0b10101,
  0b10101,
  0b10101,
  0b10101,
  0b10101,
  0b10101,
  0b10101
};



void setup() {
  // create a new character
  lcd.createChar(0, bar31);
  // create a new character
  lcd.createChar(1, bar32);
  // create a new character
  lcd.createChar(2, bar33);
  // create a new character
  lcd.createChar(3, armsDown);  
  // create a new character
  lcd.createChar(4, armsUp);  
  // set up the lcd's number of columns and rows: 
  lcd.begin(lineLength, 4);
  
  // debugging
  Serial.begin (9600);   
 
  milliseconds = 0;

  printWelcome();
  // Wait for drive to power on
  for (int k=0;k<100;k++) delay(100);
  clearLine(0);
  clearLine(1);
  clearLine(2);
  clearLine(3);

  // SPI Setup
  // have to send on master in, *slave out*
  pinMode(MISO, OUTPUT);
 
  // turn on SPI in slave mode
  SPCR |= _BV(SPE);
 
  // turn on interrupts
  SPCR |= _BV(SPIE);
 
  pos = 0;
  process_it = false;
 
  // Temperature sensor setup
  // Start up the library 
  sensors.begin(); 
}


// SPI interrupt routine
ISR (SPI_STC_vect)
{
  // Read received byte
  byte c = SPDR;
  unsigned long now = millis();
  // If last received byte is old enough, we reset the buffer index
  if (milliseconds + _SPIMinFrameTime_ < now)
  {
    pos = 0;
  }
  milliseconds = now;
  // add to buffer if room
  if (pos < sizeof buf)
   {
   buf [pos++] = c;
   
   // Process when buffer full
   if (pos + 1 == sizeof buf)
     process_it = true;
     
   }  // end of room available
}

// char buffer to int buffer
void convertBuffer(void)
{
  int ic = 0;
  for (int i=0; i<_SPIBuffSize_; i++)
  {
    unsigned long val = 0;
    for (int j=0; j<_SPINBytesPerData_; j++)
    {
      unsigned long b = (unsigned long) buf[ic++];
      val += b << (8 * j);
    }
    intBuff[i] = (long) val;
  }
}


void loop() {
  
  delay(100); 
  
  // Read temperature sensor
  sensors.requestTemperatures();
  float sensorTemp = sensors.getTempCByIndex(0);

  int totalTimeSec = 0;
  int totalTimeMin = 0;
  int nSp = 20;

  // Get values from SPI
  if (process_it)
   {
   convertBuffer();

   distance = (long)  intBuff[0];
   speed                = (float) intBuff[1];
   speed    *= 0.1;
   energyRowerRealkJoule  = (float) intBuff[2];
   energyRowerRealkJoule *= 0.001;
   totalTimeSec         = (int)   intBuff[3];
   numStrokesPerMin     = (float) intBuff[4];
   numStrokesPerMin *=0.1;
   avStrokePower        = (float) intBuff[5];
   axisMotTemp          = (int) intBuff[6];
   axisLoadTemp         = (int) intBuff[7];

   process_it = false;
   
   }  // end of flag set

  totalTimeMin = totalTimeSec / 60;
  totalTimeSec -= 60 * totalTimeMin;

  // Limit data range
  saturate(long(0), distance, long(99999999L));
  saturate(0.0f, speed, 99.9f);
  saturate(0, totalTimeMin, 999);
  saturate(0, totalTimeSec, 60);
  saturate(0.0f, energyRowerRealkJoule, 999.999f);
  saturate(0.0f, avStrokePower, 999.0f);
  saturate(0.0f, numStrokesPerMin, 99.9f);
  saturate(0, axisMotTemp, 999);
  saturate(0, axisLoadTemp, 999);
  saturate(0.0f, sensorTemp, 999.9f);
  
  
//  phase3 += h;
//  //bar(4, 3, phase3);
//  barBuffer(0, phase3);
//  barBuffer(1, phase3);
//  barBuffer(2, phase3);

  lcd.clear(); // clears and set cursor home
  
  nSp = 10;
  lcd.setCursor(0, 0);
  nSp -= lcd.print(speed, 1);
  nSp -= lcd.print(" m/s");
  printSpaces(nSp);

  nSp = 10;
  lcd.setCursor(10, 0);
  nSp -= lcd.print(distance);//, 1);
  nSp -= lcd.print(" m");
  printSpaces(nSp);

  nSp = 10;
  lcd.setCursor(0, 1);
  if (totalTimeMin < 10) nSp -= lcd.print("0");
  nSp -= lcd.print(totalTimeMin);
  nSp -= lcd.print(":");
  if (totalTimeSec < 10) nSp -= lcd.print("0");
  nSp -= lcd.print(totalTimeSec);
  printSpaces(nSp);

  nSp = 10;
  lcd.setCursor(10, 1);
  nSp -= lcd.print(energyRowerRealkJoule, 0);
  nSp -= lcd.print(" kJ");
  printSpaces(nSp);

  nSp = 10;
  lcd.setCursor(0, 2);
  nSp -= lcd.print(avStrokePower, 0);
  nSp -= lcd.print(" W");
  printSpaces(nSp);

  nSp = 10;
  lcd.setCursor(10, 2);
  nSp -= lcd.print(numStrokesPerMin, 1);
  nSp -= lcd.print(" s/min");
  printSpaces(nSp);

  nSp = 20;
  lcd.setCursor(0, 3);
  nSp -= lcd.print("T = ");
  nSp -= lcd.print(axisMotTemp, 1);
  nSp -= lcd.print(" ");
  nSp -= lcd.print(axisLoadTemp, 1);
  nSp -= lcd.print(" ");
  nSp -= lcd.print(sensorTemp, 1);
  printSpaces(nSp);


  


  
  
  if (phase3>1.1 || phase3<0.0) h*=-1.0;
}

void clearLine(int line)
{
  lcd.setCursor(0, line);
  lcd.print("                    ");
}
void printWelcome(void)
{
  clearLine(0);
  lcd.setCursor(0, 1);
  lcd.print("     BIENVENUE      ");
  clearLine(2);
  clearLine(3);
}


// Bar 
void bar(int curs, int line, float val)
{
  int nMax = 3 * (lineLength - curs);
  float nUsef = val*float(nMax);
  int nUse = round(nUsef);
  
  nUse = nUse<0 ? 0 : nUse;
  nUse = nUse>nMax ? nMax : nUse; 
  
  int nFull = nUse / 3;
  int frac = nUse % 3;
  lcd.setCursor(curs, line);
  for (byte k=0; k<nFull; k++)
  {
    lcd.write((byte) 2);
  }
  if (frac == 1) {
    lcd.write((byte) 0);
    nUse++;
  }
  if (frac == 2) {
    lcd.write((byte) 1);
    nUse++;
  }
  for (byte k=0; k<lineLength - curs - nUse; k++)
  {
    lcd.print(" ");
  }
}

// Bar 
void barBuffer(int line, float val)
{
  int nMax = 3 * (lineLength);
  float nUsef = val*float(nMax);
  int nUse = round(nUsef);
  
  nUse = nUse<0 ? 0 : nUse;
  nUse = nUse>nMax ? nMax : nUse; 
  
  int nFull = nUse / 3;
  int frac  = nUse % 3;
  lcd.setCursor(0, line);
  
  char buffer[20];
  int i = 0;
  
  for (byte k=0; k<nFull; k++)
  {
    buffer[i] = 2;
    i++;
  }
  if (frac == 1) {
    buffer[i] = 8; // 8=0
    i++;
    nFull++;
  }
  if (frac == 2) {
    buffer[i] = 1;
    i++;
    nFull++;
  }
  for (byte k=0; k<lineLength - nFull; k++)
  {
    buffer[i] = ' ';
    i++;
  }
  lcd.print(buffer);
}


void printSpaces(int n)
{
    char sp[21];
    for (int k = 0; k<n && k<20; k++)
    {
      sp[k] = ' ';
    }
    int m = n < 20 ? n : 20;
    sp[m] = '\0';
    lcd.print(sp);
}


