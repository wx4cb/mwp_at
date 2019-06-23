
#ifndef __GPS_H__
#define __GPS_H__
#include <TinyGPS++.h>

// Baud Rate of GPS
static const uint32_t GPSBaud = 9600;

// TIMEZONE
#define TIMEZONE -5

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
//SoftwareSerial ss(RXPin, TXPin);

float  gps_lat, gps_lon, gps_altitude, gps_course, gps_speed;
unsigned long gps_age, gps_time, gps_chars, gps_date_raw, gps_time_raw;
unsigned short gps_sentences, gps_failed, gps_passed;
unsigned int gps_sats, gps_hdop;
static char gps_date_str[12];
static char gps_time_str[12];
static char gps_lat_str[12];
static char gps_lon_str[12];

char* deblank(char* input);

int InitGPS(unsigned int baud) {
  Serial.println("Init GPS");
  Serial1.begin(baud);

#ifdef __TFT_H__
  // Set the font colour to be white with a black background, set text size multiplier to 1
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);

  tft.println("Initialise GPS");
#endif
}

void UpdateGPS(void) {
  // Dispatch incoming characters
  while (Serial1.available() > 0)
    gps.encode(Serial1.read());

  /*
       parse object
  */

  if (gps.location.isUpdated())  {
    gps_age = gps.location.age();
    gps_lat = gps.location.lat();
    gps_lon = gps.location.lng();
    // convert to char array
    dtostrf(gps_lat, 10, 5, deblank(gps_lat_str));
    dtostrf(gps_lon, 10, 5, deblank(gps_lon_str));
  }

  else if (gps.date.isUpdated())  {
    gps_date_raw = gps.date.value();
    sprintf(gps_date_str, "%d.%d.%d", gps.date.month(), gps.date.day(), gps.date.year());
  }

  else if (gps.time.isUpdated())
  {
    gps_time_raw = gps.time.value();
    int gps_hour = gps.time.hour();
    int gps_local_hour = gps_hour += TIMEZONE;
    sprintf(gps_time_str, "%d.%d.%d", gps_local_hour, gps.time.minute(), gps.time.second());
  }

  else if (gps.speed.isUpdated())
  {
    gps_speed = gps.speed.mph();
  }

  else if (gps.course.isUpdated())
  {
    gps_course = gps.course.deg();
  }

  else if (gps.altitude.isUpdated())
  {
    gps_altitude = gps.altitude.feet();
  }

  else if (gps.satellites.isUpdated())
  {
    gps_sats = gps.satellites.value();
  }

  else if (gps.hdop.isUpdated())
  {
    gps_hdop = gps.hdop.hdop();
  }


  gps_chars = gps.charsProcessed();
  gps_sentences = gps.sentencesWithFix();
  gps_failed = gps.failedChecksum();
  gps_passed = gps.passedChecksum();

}

char* deblank(char* input)
{
    int i,j;
    char *output=input;
    for (i = 0, j = 0; i<strlen(input); i++,j++)          
    {
        if (input[i]!=' ')                           
            output[j]=input[i];                     
        else
            j--;                                     
    }
    output[j]=0;
    return output;
}

void DumpGPS(void) {

}
#endif
