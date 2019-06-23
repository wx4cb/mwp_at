

#ifndef __SDCARD_H__
#define __SDCARD_H__
/*
   SD Card Defs
*/
#include <SD.h>

// SD chip select
#define SDC_CS 53

File SD_root;

int InitSDCard(void);
void printDirectory(File, int);

int InitSDCard(void) {

  Serial.print(F("Initialising SD card..."));
  if (!SD.begin(SDC_CS)) {
    Serial.println(F("FAILED!"));
    return (-1);
  } else { Serial.println("SUCCESS"); }

  SD_root = SD.open("/");

  printDirectory(SD_root, 0);

  Serial.println("done!");
  return (0);
}

void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

#endif
