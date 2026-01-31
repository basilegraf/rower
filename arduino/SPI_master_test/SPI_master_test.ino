
 
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
  
  byte command = 11;
  byte data = 0;
  

  SPI.begin();
  SPI.beginTransaction(SPISettings(625000, MSBFIRST, SPI_MODE0));
  


  while (true)
  {
    //delay(500);
    digitalWrite(_ACCURET_SS_, LOW); // Select accuret
    data = SPI.transfer(command);
    digitalWrite(_ACCURET_SS_, HIGH); // Deselect accuret
    //Serial.print(command);
    //Serial.print(" -> ");
    //Serial.print(data);
    //Serial.print(" ");
    //Serial.print(SPCR);
    //Serial.print("\n");
    command++;
  }

}










