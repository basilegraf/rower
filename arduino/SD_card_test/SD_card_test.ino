
 
// SPI Slave:
// https://forum.arduino.cc/index.php?topic=52111.0

// 1-Wire & Temp sensor:
// https://create.arduino.cc/projecthub/TheGadgetBoy/ds18b20-digital-temperature-sensor-and-arduino-9cc806


//#include <LiquidCrystal.h>
//#include <OneWire.h> 
//#include <DallasTemperature.h>
#include <SPI.h>
#include <SD.h>
#include "pins_arduino.h"

#include "saturate.h"

// SPI configurations stuff
#define _SPIBuffSize_ 8
#define _SPINBytesPerData_ 4
#define _SPIMinFrameTime_ 50 // There should be at least 50ms between frames

// Pins
#define _SPI_CLOCK_ 13 
#define _ACCURET_SS_ 14
#define _SD_CARD_SS_ 10 // The SD library can use another pin but pin 10 (HW SS) must be set as outpu


// byte and long buffers to receive SPI data
byte buf [_SPIBuffSize_ * _SPINBytesPerData_];
long intBuff [_SPIBuffSize_]; // long: 4 bytes


// SD card variables

Sd2Card card;
SdVolume volume;
SdFile root;


//void setup() {
//  
//  // debugging
//  Serial.begin (9600);
//  while (!Serial) // wait for serial port to connect. Needed for native USB port only 
// 
//
//  // SPI Setup
//  //pinMode(MISO, INPUT);
//  //pinMode(MOSI, OUTPUT);
//  //pinMode(_SPI_CLOCK_, OUTPUT);
//  pinMode(_ACCURET_SS_, OUTPUT);
//  pinMode(_SD_CARD_SS_, OUTPUT);
//
//  digitalWrite(_ACCURET_SS_, HIGH); // Deselect accuret
//  digitalWrite(_SD_CARD_SS_, HIGH); // Deselect accuret
//}


void printSPCR()
{
  // https://www.arnabkumardas.com/arduino-tutorial/spi-register-description/
  // https://github.com/arduino/ArduinoCore-avr/tree/master/libraries/SPI/src
  byte spcrVal = SPCR;


  if (SPCR & (1 < 6))
    Serial.println("SPI enabled");
  else
    Serial.println("SPI disabled");
    
    if (SPCR & (1 < 7))
    Serial.println("    Interupt enabled");
  else
    Serial.println("    Interupt disabled");

  if (SPCR & (1 < 5))
    Serial.println("    LSB first");
  else
    Serial.println("    MSB first");

  if (SPCR & (1 < 4))
    Serial.println("    Master mode");
  else
    Serial.println("    Slave mode");

  if (SPCR & (1 < 3))
    Serial.println("    Clock idle when HIGH");
  else
    Serial.println("    Clock idle when LOW");

  if (SPCR & (1 < 2))
    Serial.println("    Sample on clock falling edge");
  else
    Serial.println("    Sample on clock rising edge");
    
  byte spiSpeedDiv = (SPCR & 0x3);

  long freq;
  if (SPI2X == 0)
  {
    freq = F_CPU >> (spiSpeedDiv + 2);
  }
  else
  {
    freq = F_CPU >> (spiSpeedDiv + 1);
  }
  

  
  Serial.print("    SPI speed ");
  Serial.print(spiSpeedDiv, HEX);
  Serial.print(" -> ");
  Serial.print(freq, DEC);
  Serial.println("Hz");
  
  
  
  
  
  //Serial.print("SPCR = ");
  //Serial.println(SPCR, BIN);
}


void setup() {


  pinMode(_ACCURET_SS_, OUTPUT);
  pinMode(_SD_CARD_SS_, OUTPUT);

  digitalWrite(_ACCURET_SS_, HIGH); // Deselect accuret
  digitalWrite(_SD_CARD_SS_, HIGH); // Deselect accuret



  // Open serial communications and wait for port to open:

  Serial.begin(9600);

  while (!Serial) {

    ; // wait for serial port to connect. Needed for native USB port only

  }

  if (false)
  {

    printSPCR();
  
    Serial.print("\nInitializing SD card...");
  
    // we'll use the initialization code from the utility libraries
  
    // since we're just testing if the card is working!
  
    if (!card.init(SPI_HALF_SPEED, _SD_CARD_SS_)) {
  
      Serial.println("initialization failed. Things to check:");
  
      Serial.println("* is a card inserted?");
  
      Serial.println("* is your wiring correct?");
  
      Serial.println("* did you change the chipSelect pin to match your shield or module?");
  
      while (1);
  
    } else {
  
      Serial.println("Wiring is correct and a card is present.");
  
    }
  
    printSPCR();
  
    // print the type of card
  
    Serial.println();
  
    Serial.print("Card type:         ");
  
    switch (card.type()) {
  
      case SD_CARD_TYPE_SD1:
  
        Serial.println("SD1");
  
        break;
  
      case SD_CARD_TYPE_SD2:
  
        Serial.println("SD2");
  
        break;
  
      case SD_CARD_TYPE_SDHC:
  
        Serial.println("SDHC");
  
        break;
  
      default:
  
        Serial.println("Unknown");
  
    }
  
    // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  
    if (!volume.init(card)) {
  
      Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
  
      while (1);
  
    }
  
    Serial.print("Clusters:          ");
  
    Serial.println(volume.clusterCount());
  
    Serial.print("Blocks x Cluster:  ");
  
    Serial.println(volume.blocksPerCluster());
  
    Serial.print("Total Blocks:      ");
  
    Serial.println(volume.blocksPerCluster() * volume.clusterCount());
  
    Serial.println();
  
    // print the type and size of the first FAT-type volume
  
    uint32_t volumesize;
  
    Serial.print("Volume type is:    FAT");
  
    Serial.println(volume.fatType(), DEC);
  
    volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  
    volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  
    volumesize /= 2;                           // SD card blocks are always 512 bytes (2 blocks are 1KB)
  
    Serial.print("Volume size (Kb):  ");
  
    Serial.println(volumesize);
  
    Serial.print("Volume size (Mb):  ");
  
    volumesize /= 1024;
  
    Serial.println(volumesize);
  
    Serial.print("Volume size (Gb):  ");
  
    Serial.println((float)volumesize / 1024.0);
  
    Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  
    root.openRoot(volume);
  
    // list all files in the card with date and size
  
    root.ls(LS_R | LS_DATE | LS_SIZE);
  
    printSPCR();
    
    root.close();
  
    printSPCR();
  }
}



void loop(void)
{
  byte k = 0;
  byte command = 0;
  byte lastCommand = 0;
  byte challenge = 0;
  byte reply = 0;
  byte errorCount = 0;


  Serial.println("Loop");
  printSPCR();
  SPI.begin();
  printSPCR();
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
  printSPCR();
  
  for (k =0; k<254; k++)
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

    if (k==253)
    {
      Serial.print("Error count : ");
      Serial.print(errorCount);
      Serial.print("\n");
    }

    delayMicroseconds(500);
    
  }

  printSPCR();
  SPI.endTransaction();
  printSPCR();
  SPI.end();

  while(1); // stop here
  
}

//void loop() {
//  
//  delay(100); 
//
//  byte k = 0;
//  byte command = 0;
//  byte lastCommand = 0;
//  byte challenge = 0;
//  byte reply = 0;
//  byte errorCount = 0;
//  
//
//  SPI.begin();
//  SPI.beginTransaction(SPISettings(625000, MSBFIRST, SPI_MODE0));
//  //SPI.beginTransaction(SPISettings(312500, MSBFIRST, SPI_MODE0));
//  
//
//
//  while (true)
//  {
//    command = k;
//    
//    digitalWrite(_ACCURET_SS_, LOW); // Select accuret
//    reply = SPI.transfer(command);
//    digitalWrite(_ACCURET_SS_, HIGH); // Deselect accuret
//
//    if (k == 0)
//      errorCount = 0;
//    else
//    {
//      challenge = (lastCommand + byte(77)) & 0xFF;
//      if (reply != challenge) ++errorCount;
//    }
//
//    lastCommand = command;
//    k++;
//
//    if (k==255)
//    {
//      Serial.print("Error count : ");
//      Serial.print(errorCount);
//      Serial.print("\n");
//    }
//
//    delayMicroseconds(500);
//    
//  }
//
//}










