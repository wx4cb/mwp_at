#include <TinyGPS.h>

#include <Wire.h>
#include <HMC5883L.h>
#include <Servo.h>
#include <SoftwareSerial.h>


HMC5883L compass;

// This is our continuous rotation servo
Servo servo;
int NORTH = 0;
int SERVO_STOP = 90;  // change this value to achieve minimum rotation!
int SERVO_PAN = 9;

// Pi for calculations - not the raspberry type
const float Pi = 3.14159;

static int target;
static int lasttarget;


void setup(void) 
{
  Serial.begin(115200);
  Serial.println("MWPTools Antenna Tracker Remote - v1.0.1"); Serial.println("");
  
 // Initialize HMC5883L
  Serial.println("Initialize HMC5883L");
  while (!compass.begin())
  {
    Serial.println ( "HMC5883L not found, check connection!" ) ;
    delay(500);
  }
 
  // Set the measuring range
  compass.setRange(HMC5883L_RANGE_1_3GA);
 
  // Set the operating mode
  compass.setMeasurementMode(HMC5883L_CONTINOUS);
 
  // Setting the measurement frequency
  compass.setDataRate(HMC5883L_DATARATE_15HZ);
 
  // The number of averaged samples
  compass.setSamples(HMC5883L_SAMPLES_4);

  servo.attach(SERVO_PAN);  // attaches the servo on pin 9 to the servo object 
  servo.write(SERVO_STOP);  
  
  MoveServo(NORTH);
}

int GetHeading(void) {
 // Downloading normalized vectors
  Vector norm = compass.readNormalize();
 
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
  float headingDegrees = heading * 180/M_PI;

  return(headingDegrees);
}

void MoveServo(int t) {
  int current = GetHeading();
  
  // Calculate the error between tha measured heading and the target heading.
  float error = current - t;
  if (error > 180)
  {
    error = error - 360;  // for angles > 180, correct in the opposite direction.
  }
  // A non-zero difference between the heading and the 
  // targetHeading will bias the servoNeutral value and 
  // cause the servo to rotate back toward the targetHeading.
  // The divisor is to reduce the reaction speed and avoid oscillations
  servo.write(SERVO_STOP + error / 4 );

  Serial.print("Current: "); Serial.print(current);
  Serial.print(" Target: "); Serial.print(t);
  Serial.print(" Error: "); Serial.println(error);
}


String inString = "";    // string to hold input

void loop(void) 
{
 
// Read serial input:
  while (Serial.available() > 0) {
    int inChar = Serial.read();
    if (isDigit(inChar)) {
      // convert the incoming byte to a char and add it to the string:
      inString += (char)inChar;
    }
    // if you get a newline, print the string, then the string's value:
    if (inChar == '\n') {
      lasttarget = target;
      target = inString.toInt();
      if (target >= 360) { target = 0; }
      target = constrain(target, 0, 359);
      
      Serial.print("MAIN: last-target: "); Serial.print(lasttarget);
      Serial.print(" - Target: "); Serial.println(target);
      
      inString = "";
    }
  }

  MoveServo(target);

  delay(50);
}
