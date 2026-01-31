
 
// SPI Slave:
// https://forum.arduino.cc/index.php?topic=52111.0

// 1-Wire & Temp sensor:
// https://create.arduino.cc/projecthub/TheGadgetBoy/ds18b20-digital-temperature-sensor-and-arduino-9cc806


//#include <LiquidCrystal.h>
//#include <OneWire.h> 
//#include <DallasTemperature.h>
#include <SPI.h>
#include "pins_arduino.h"

#include "saturate.h"

// SPI configurations stuff
#define _SPIBuffSize_ 8
#define _SPINBytesPerData_ 4
#define _SPIMinFrameTime_ 50 // There should be at least 50ms between frames

// Pins
#define _SPI_CLOCK_ 13 
#define _ACCURET_SS_ 14


// byte and long buffers to receive SPI data
byte buf [_SPIBuffSize_ * _SPINBytesPerData_];
long intBuff [_SPIBuffSize_]; // long: 4 bytes




void setup() {
  
  // debugging
  Serial.begin (9600);   
 

  // SPI Setup
  //pinMode(MISO, INPUT);
  //pinMode(MOSI, OUTPUT);
  //pinMode(_SPI_CLOCK_, OUTPUT);
  pinMode(_ACCURET_SS_, OUTPUT);

  digitalWrite(_ACCURET_SS_, HIGH); // Deselect accuret
 
  SPI.begin();

 
 
}





void loop() {
  
  delay(100); 

  byte k = 0;
  byte command = 0;
  byte lastCommand = 0;
  byte challenge = 0;
  byte reply = 0;
  byte errorCount = 0;
  

  SPI.begin();
  SPI.beginTransaction(SPISettings(625000, MSBFIRST, SPI_MODE0));
  //SPI.beginTransaction(SPISettings(312500, MSBFIRST, SPI_MODE0));
  


  while (true)
  {
    command = k;
    
    digitalWrite(_ACCURET_SS_, LOW); // Select accuret
    reply = SPI.transfer(command);
    digitalWrite(_ACCURET_SS_, HIGH); // Deselect accuret

    if (k == 0)
      errorCount = 0;
    else
    {
      challenge = (lastCommand + byte(77)) & 0xFF;
      if (reply != challenge) ++errorCount;
    }

    lastCommand = command;
    k++;

    if (k==255)
    {
      Serial.print("Error count : ");
      Serial.print(errorCount);
      Serial.print("\n");
    }

    delayMicroseconds(500);
    
  }

}










