
// Setup IMU
// I2C interface by default
#include "Wire.h"
#include "SparkFunIMU.h"
#include "SparkFunLSM303C.h"
#include "LSM303CTypes.h"

#ifndef __UTIL_H__
  #include "util.h"
#endif

// #define DEBUG 1 in SparkFunLSM303C.h turns on debugging statements.
// Redefine to 0 to turn them off.
LSM303C myIMU;
#define SCALE 2  // accel full-scale, should be 2, 4, or 8

#define X 0
#define Y 1
#define Z 2

float accel[3];  // we’ll store the raw acceleration values here
float mag[3];  // raw magnetometer values stored here
float realAccel[3];  // calculated acceleration values here
float pitch_roll[2];
int LSM303_Temp = 0;

int GetHeading(int MagX, int MagY) {

  // Calculating the direction (rad)
  float heading = atan2(MagX, MagY);

  // Set the declination angle for Bytom 4'26E (positive)
  // Formula: (deg + (min / 60.0)) / (180 / M_PI);
  float declinationAngle = (4.0 + (26.0 / 60.0)) / (180 / M_PI);
  heading += declinationAngle;

  // Correction katow
  if (heading < 0)
  {
    heading += 2 * PI;
  }

  if (heading > 2 * PI)
  {
    heading -= 2 * PI;
  }

  // Change radians to degrees
  float headingDegrees = heading * 180/M_PI;

  return(headingDegrees);
}

// Program to read LSM303 and put its data into globals
void ReadLSM303() {
   // Get Mag Values
  mag[X]= myIMU.readMagX();
  mag[Y] = myIMU.readMagY();
  mag[Z] = myIMU.readMagZ();

  // Get Accelerometer Values
  accel[X]= myIMU.readAccelX();
  accel[Y] = myIMU.readAccelY();
  accel[Z] = myIMU.readAccelZ();

// calculate real acceleration values, in units of g
   for (int i=0; i<3; i++)
    realAccel[i] = accel[i] / pow(2, 15) * SCALE;

   LSM303_Temp = int(myIMU.readTempF());

   // Calculate pitch roll angles
   //pitch_roll[0] = arctan(accel[X] / (SQRT(accel[Y]^2 + accel[Z]^2))
   //pitch_roll[1] = arctan(accel[Y]/ (SQRT(accel[X]^2 + accel[Z]^2))
}

float LSM303_GetHeading(float * magValue)
{
  // see section 1.2 in app note AN3192
  float heading = 180 * atan2(magValue[Y], magValue[X])/PI;  // assume pitch, roll are 0

  if (heading <0)
    heading += 360;

  return heading;
}

float LSM303_GetTiltHeading(float * magValue, float * accelValue)
{
  // see appendix A in app note AN3192
  float pitch = asin(-accelValue[X]);
  float roll = asin(accelValue[Y]/cos(pitch));

  float xh = magValue[X] * cos(pitch) + magValue[Z] * sin(pitch);
  float yh = magValue[X] * sin(roll) * sin(pitch) + magValue[Y] * cos(roll) - magValue[Z] * sin(roll) * cos(pitch);
  float zh = -magValue[X] * cos(roll) * sin(pitch) + magValue[Y] * sin(roll) + magValue[Z] * cos(roll) * cos(pitch);

  float heading = 180 * atan2(yh, xh)/PI;
  if (yh >= 0)
    return heading;
  else
    return (360 + heading);
}

void DoIMUStatus(void) {
  Serial.println("\nIMU STATUS\n=============");

  char buf[100];
  char tmp[3][20];

  // Current IMU Die Temp
  printf("IMU Temp: %d\n", LSM303_Temp);

  // Current ACC/GYRO/MAG Values
  floatToString(tmp[X], accel[X], 2, true);
  floatToString(tmp[Y], accel[Y], 2, true);
  floatToString(tmp[Z], accel[Z], 2, true);

  printf("ACC (x/y/z): %s\t%s\t%s\n", tmp[X], tmp[Y], tmp[Z]);

  floatToString(tmp[X], mag[X], 2, true);
  floatToString(tmp[Y], mag[Y], 2, true);
  floatToString(tmp[Z], mag[Z], 2, true);
  printf("MAG (x/y/z): %s\t%s\t%s\n", tmp[X], tmp[Y], tmp[Z]);

  char head[10];
  char tilthead[10];

  floatToString(head, LSM303_GetHeading(mag), 2, true);
  printf("Heading: %s\n\n", head);
}
