#include <SPI.h>
#include "SdFat.h"
#include "sdios.h"
#include "pins_arduino.h"

#define CHIP_SELECT_SD 10
#define CHIP_SELECT_ACCURET 14

// Test with reduced SPI speed for breadboards.  SD_SCK_MHZ(4) will select
// the highest speed supported by the board that is not over 4 MHz.
// Change SPI_SPEED to SD_SCK_MHZ(50) for best performance.
#define SPI_SPEED_SD SD_SCK_MHZ(4)

// AccurEt is slow as slave
#define SPI_SPEED_ACCURET 500000 


SdFat32 sd;
File32 file;

// Serial streams
ArduinoOutStream cout(Serial);

void setup() 
{
  // Serial setup
  Serial.begin(9600);

  // Wait for USB Serial
  while (!Serial) 
  {
    yield();
  }

  // SD card and open file
  
  cout << F("\nSPI pins:\n");
  cout << F("MISO: ") << int(MISO) << endl;
  cout << F("MOSI: ") << int(MOSI) << endl;
  cout << F("SCK:  ") << int(SCK) << endl;
  cout << F("SS:   ") << int(SS) << endl;
  cout << F("SDCARD_SS_PIN:   ") << int(CHIP_SELECT_SD) << endl;

  // Deselect accuret 
  pinMode(CHIP_SELECT_ACCURET, OUTPUT);
  digitalWrite(CHIP_SELECT_ACCURET, HIGH);

  if (!sd.begin(CHIP_SELECT_SD, SPI_SPEED_SD)) 
  {
    cout << F("Failed to init SD card") << endl;
    while(1); // Stop here
  }

  cout << F("\nCard successfully initialized.\n");
  
  file = sd.open("test.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (file) 
  {
    Serial.println("File opened");
  //  Serial.println("done.");
  } else 
  {
    Serial.println("error opening test.txt");
  }


  // SPI setting for accuret communication
  SPI.begin();
  
}



void loop() 
{
  if (file)
  {
    Serial.println("Loop begin");
    file.println("Loop begin");
  }


  byte command, lastCommand, response, expected;
  lastCommand = 0;
  for (byte k = 0; k < 10; k++)
  {
    
    // Send data to accuret
    digitalWrite(CHIP_SELECT_SD, HIGH);
    digitalWrite(CHIP_SELECT_ACCURET, LOW);
    SPI.beginTransaction(SPISettings(SPI_SPEED_ACCURET, MSBFIRST, SPI_MODE0));
    command = k;
    response = SPI.transfer(command); // call multiple times of use buffered version
    SPI.endTransaction();
    digitalWrite(CHIP_SELECT_ACCURET, HIGH);
    digitalWrite(CHIP_SELECT_SD, LOW);
    
    expected = (lastCommand + byte(77)) & 0xFF;
    if (file)
    {
      Serial.print(lastCommand);
      Serial.print(" -> ");
      Serial.print(response);
      Serial.print(", expected : ");
      Serial.println(expected);

      file.print(lastCommand, DEC);
      file.print(" -> ");
      file.print(response, DEC);
      file.print(", expected : ");
      file.println(expected, DEC);
    }

    lastCommand = command;
  }

  if (file)
  {
    Serial.println("Loop end");
    file.println("Loop end");
  }

  SPI.end();

  file.close();
  sd.end();

  while(1);
}

