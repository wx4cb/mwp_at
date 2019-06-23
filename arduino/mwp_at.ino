/*
  MWP Antenna Tracker, Remote Module.

  Uses HMC-5883 I2c Compass and also UART GPS

  LCD and button interface provided by LCD Shield

  GPS is set to TX/RX on pins 10/11

*/
#include "mwp_at.h"


/*
   Main setup routine
*/

void setup(void)
{
  Serial.begin(115200);
  Serial.println("MWPTools Antenna Tracker Remote - v1.0.1");
  Serial.println("");

#ifdef __LCDSHIELD_H__
  SetupLCDShield();
  lcd.clear();
#endif

#ifdef __SDCARD_H__
  InitSDCard();
#endif
#ifdef __TFT_H__
  InitTFT();
#endif

#ifdef __COMPASS_H__
  if (InitCompass() == -1) {
    TFT_Print(TFT_CENTER_Y - 125, TFT_CENTER_X - 75, "Compass Failure", TFT_RED, TFT_BLACK, 3);
    while (1) {
      ;
    }
  }
#endif

#ifdef __GPS_H__
  InitGPS(9600);
#endif

#ifdef __SERVOS_H__
  InitServos();

#endif

#ifdef __STEPPER_H__
  InitStepper();
#endif

#ifdef __TFT_H__
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(1);

  tft.println("Initialisation Complete.");
#endif

  Serial.println("Initialisation Complete.");
}

String inString = "";    // string to hold input

/*
   Main loop
*/
void loop(void)
{
  char buf[80];

  /*
     Read serial input:
     We are lookin for a number from the input port as a degree heading to go to
  */
  while (Serial.available() > 0) {
    int inChar = Serial.read();
    if (isDigit(inChar)) {
      // convert the incoming byte to a char and add it to the string:
      inString += (char)inChar;
    }
    // if you get a newline, print the string, then the string's value:
    if (inChar == '\n') {
      lasttarget = pan_target;
      pan_target = inString.toInt();
      /*
         Make sure that the target is not > 359 or < 0
      */
      if (pan_target > 359) {
        pan_target = 0;
      }
      if (pan_target < 0) {
        pan_target = 0;
      }

      /*
          constrain the target to be within 0 - 359, this is just a secndary check.
      */
      pan_target = constrain(pan_target, 0, 359);

      Serial.print(F("MAIN: last-target: ")); Serial.print(lasttarget);
      Serial.print(F(" - Target: ")); Serial.println(pan_target);

      inString = "";
    }
  }

#ifdef __TFT_H__
  // This should only run on first runt through
  if (TFT_FIRST_UPDATE) {
    UpdateTFT();
  }

#endif

  // Process IMU Data
  GetIMU();

#ifdef __LCDSHIELD_H__
  ProcessMenu();
#endif

#ifdef __GPS_H__
  UpdateGPS();

#endif

#ifdef __SERVOS_H__
  // Move the servo
  MoveServo(SERVO_PAN, pan_target);
#endif

#ifdef __STEPPER_H__
  //Serial.println("Moving Stepper");
  PanStepperObj.run();
  TiltStepperObj.run();
  // Move Stepper
  MovePanStepper(pan_target);
#endif

#ifndef __GPS_H__
  sprintf(buf, "Current: %d - Target: %d - Error: %d\n", current, target, servo_error);
#else

  sprintf(buf, "%s %s: Current: %d - Target: %d - Error: %d - LAT: %s - LON: %s - Sats: %d - Sentences: %d - Chars: %d\n",
          gps_date_str, gps_time_str, pan_current, pan_target, pan_servo_error, gps_lat_str, gps_lon_str, gps_sats, gps_sentences, gps_chars);

#endif
  Serial.println(buf);

  delay(150);
}
