#include <SPI.h>
#include "SdFat.h"
#include "sdios.h"


#define CHIP_SELECT_SD 10
#define CHIP_SELECT_ACCURET 14

// Test with reduced SPI speed for breadboards.  SD_SCK_MHZ(4) will select
// the highest speed supported by the board that is not over 4 MHz.
// Change SPI_SPEED to SD_SCK_MHZ(50) for best performance.
#define SPI_SPEED_SD SD_SCK_MHZ(4)

SdFat32 sd;
File32 file;

// Serial streams
ArduinoOutStream cout(Serial);

void setup() 
{
  Serial.begin(9600);

  // Wait for USB Serial
  while (!Serial) 
  {
    yield();
  }
  cout << F("\nSPI pins:\n");
  cout << F("MISO: ") << int(MISO) << endl;
  cout << F("MOSI: ") << int(MOSI) << endl;
  cout << F("SCK:  ") << int(SCK) << endl;
  cout << F("SS:   ") << int(SS) << endl;
  cout << F("SDCARD_SS_PIN:   ") << int(CHIP_SELECT_SD) << endl;

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
  if (file) {
    Serial.print("Writing to test.txt...");
    file.println("testing 1, 2, 3.");
    // close the file:
 //   file.close();
  //  Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  
}



void loop() {

  for (byte k = 0; k < 10; k++)
  {
    if (file)
    {
      Serial.println(k);
      file.println(k);
    }
  }

  file.close();
  sd.end();

  while(1);
}

