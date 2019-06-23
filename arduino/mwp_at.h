#ifndef __MWP_AT_H__
#define __MWP_AT_H__
#include <SoftwareSerial.h>

#include "tft.h"
#include "sdcard.h"
#include "servos.h"
//#include "lcdshield.h"
#include "compass.h"
#include "gps.h"
//#include "steppermotor.h"
#include "lang.h"

#ifdef __TFT_H__
extern TFT_HX8357 tft;
#endif

// Pi for calculations - not the raspberry type
const float Pi = 3.14159;

/*
   Target bearing reqiured and last target bearing received
*/
static int pan_current;
static int tilt_current;
static int pan_target;
static int tilt_target;
static int lasttarget;
static int pan_servo_error;
static int tilt_servo_error;

const char __PROGNAME__[] = "MWP Antenna Tracker Remote";
const char __COPYLEFT__[] = "(c) 2019, Charles Blackburn";
const char __PROGVERS__[] = "v1.0.5";


#endif
