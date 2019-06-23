#ifndef __LSM303__
#define __LSM303__

#include <DebugMacros.h>
#include <LSM303CTypes.h>
#include <SparkFunIMU.h>
#include <SparkFunLSM303C.h>

#include <math.h>


LSM303C myIMU;
float a_x;
float a_y;
float a_z;

float m_x;
float m_y;
float m_z;

int InitCompass() {
  Wire.begin();//set up I2C bus, comment out if using SPI mode

  Wire.setClock(400000L);//clock stretching, comment out if using SPI mode

  if (myIMU.begin() != IMU_SUCCESS)
  {
    Serial.println("Failed setup.");
    while (1);
  }
}

void GetIMU(void) {
  a_x = myIMU.readAccelX();
  a_y = myIMU.readAccelY();
  a_z = myIMU.readAccelZ();

  m_x = myIMU.readMagX();
  m_y = myIMU.readMagX();
  m_z = myIMU.readMagX();
}

int GetTherm(void) {
  return (int(myIMU.readTempF()));
}

int GetTiltHeading(void) {
  char str_x[6];
  char str_y[6];
  char str_z[6];
  char LCDText[16];

  //Get all parameters
  Serial.print("\nAccelerometer:\n");
  Serial.print(" X = ");
  Serial.println(a_x, 4);
  Serial.print(" Y = ");
  Serial.println(a_y, 4);
  Serial.print(" Z = ");
  Serial.println(a_z, 4);

  Serial.print("\nMagnetometer:\n");
  Serial.print(" X = ");
  Serial.println(m_x, 4);
  Serial.print(" Y = ");
  Serial.println(m_y, 4);
  Serial.print(" Z = ");
  Serial.println(m_z, 4);
  /*
    // see appendix A in app note AN3192
    float pitch = asin(-a_x);
    float roll = asin(a_y/cos(pitch));

    float pitch = arctan(a_x / (SQRT(a_y^2 + a_z^2))
    float roll = arctan(a_y / (SQRT(a_x^2 + a_z^2))

    Serial.print("Pitch: "); Serial.print(pitch);
    Serial.print(" - Roll: "); Serial.println(roll);
    float xh = m_x * cos(pitch) + m_z * sin(pitch);
    float yh = m_x * sin(roll) * sin(pitch) + m_y * cos(roll) - m_z * sin(roll) * cos(pitch);
    float zh = -m_x * cos(roll) * sin(pitch) + m_y * sin(roll) + m_z * cos(roll) * cos(pitch);



    Serial.print("\nTilt:\n");
    Serial.print(" X = ");
    Serial.println(xh, 4);
    Serial.print(" Y = ");
    Serial.println(yh, 4);
    Serial.print(" Z = ");
    Serial.println(zh, 4);
  */
  float pitch = atan(a_x / (sqrt(int(a_y) ^ 2 + int(a_z) ^ 2)));
  float roll = atan(a_y / (sqrt(int(a_x) ^ 2 + int(a_z) ^ 2)));
 Serial.print("Pitch: "); Serial.print(pitch);
Serial.print(" - Roll: "); Serial.println(roll);
  float heading = 180 * atan2(m_y, m_x) / M_PI;

  Serial.print("Heading: "); Serial.println(heading);

  if (m_y >= 0)
    return heading;
  else
    return (360 + heading);
  }

int GetHeading(void) {

  // Calculating the direction (rad)
  float heading = (atan2(m_y, m_x));// * 180) / Pi;

  // Set the declination angle for Bytom 4'26E (positive)
  // Formula: (deg + (min / 60.0)) / (180 / M_PI);
  float declinationAngle = (4.0 + (26.0 / 60.0)) / (180 / M_PI);
  //heading += declinationAngle;

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
#endif
