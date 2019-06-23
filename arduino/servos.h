#ifndef __SERVOS_H__
#define __SERVOS_H__
#include <Servo.h>

/*
   Servo Setup
*/
// This is our continuous rotation servo
Servo servo_pan;
Servo servo_tilt;
#define SERVO_PAN  9
#define SERVO_TILT  8
#define NORTH 0
#define SERVO_STOP 90  // change this value to achieve minimum rotation!


extern int pan_servo_error;
extern int tilt_servo_error;
extern int pan_current;
extern int tilt_current;

extern int GetHeading(void);
extern int GetTiltHeading(void);
void MoveServo(int, int);
/*
   Initialise the servos
*/
int InitServos(void) {
    Serial.println("Init Servos");

  servo_pan.attach(SERVO_PAN);  // attaches the servo on pin 9 to the servo object
  servo_tilt.attach(SERVO_TILT);  // attaches the servo on pin 8 to the servo object

#ifdef __TFT_H__
  // Set the font colour to be white with a black background, set text size multiplier to 1
  tft.setTextColor(TFT_WHITE,TFT_BLACK);  
  tft.setTextSize(1);

  tft.println("Initialise Servos");
#endif

    // Move servos to "North"
  MoveServo(SERVO_PAN, NORTH);
  
  servo_pan.write(SERVO_STOP);
  servo_tilt.write(SERVO_STOP);
}

/*
    Move the servo to the desired heading given at "t"

    Gets the current heading from the compass
*/

void MoveServo(int pt, int t) {

  // A non-zero difference between the heading and the
  // targetHeading will bias the servoNeutral value and
  // cause the servo to rotate back toward the targetHeading.
  // The divisor is to reduce the reaction speed and avoid oscillations
  switch (pt) {
    case SERVO_PAN:
      // Calculate the error between tha measured heading and the target heading.
      pan_servo_error = pan_current - t;
      if (pan_servo_error > 180)
      {
        pan_servo_error = pan_servo_error - 360;  // for angles > 180, correct in the opposite direction.
      }
      servo_pan.write(SERVO_STOP + pan_servo_error / 4 );
      break;
    case SERVO_TILT:
      servo_tilt.write(SERVO_STOP + pan_servo_error / 4 );
      break;

  }
}

#endif
