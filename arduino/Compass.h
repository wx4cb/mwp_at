#ifndef __COMPASS_H__
#define __COMPASS_H__
#include <Wire.h>
#include <HMC5883L.h>

/*
    COMPASS AND GPS SEtup
*/
HMC5883L compass;
Vector norm;

#ifdef __TFT_H__
extern TFT_HX8357 tft;
#endif

float a_x;
float a_y;
float a_z;

float m_x;
float m_y;
float m_z;

extern int pan_servo_error;
extern int tilt_servo_error;
extern int pan_current;
extern int tilt_current;

int InitCompass(void) {
  int init_count = 0;
  int init_tries = 3;         // Retry counter

  Serial.println("Initialize HMC5883L");

#ifdef __TFT_H__
  Serial.println(" compass tft check");
  // Set the font colour to be white with a black background, set text size multiplier to 1
  tft.setTextColor(TFT_WHITE,TFT_BLACK);  
  tft.setTextSize(1);

  tft.println("Initialise Compass");
#endif

  // Initialize HMC5883L
  while (!compass.begin())
  {
    Serial.println ( "HMC5883L not found, check connection!" ) ;
    delay(125);
    // if tried > init_tries, return -1 as a fail
    if (init_count++ > init_tries) {
      return (-1);
    }
  }

  // Set the measuring range
  compass.setRange(HMC5883L_RANGE_1_3GA);

  // Set the operating mode
  compass.setMeasurementMode(HMC5883L_CONTINOUS);

  // Setting the measurement frequency
  compass.setDataRate(HMC5883L_DATARATE_15HZ);

  // The number of averaged samples
  compass.setSamples(HMC5883L_SAMPLES_4);

  return (0);
}

/*
   Get the heading from the compass, try and compensate for tilt and declination
*/
int GetHeading(void) {


  // Calculating the direction (rad)
  float heading = atan2(norm.YAxis, norm.XAxis);

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
  float headingDegrees = heading * 180 / M_PI;

  return (headingDegrees);
}

extern int GetTiltHeading(void) {
  return (GetHeading());
}
void GetIMU(void) {
  // Downloading normalized vectors
  norm = compass.readNormalize();
  pan_current = GetHeading();
  //Serial.println(pan_current);
}
#endif
