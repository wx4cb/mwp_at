#include <MegunoLink.h>
#include <CommandHandler.h>
#include <TCPCommandHandler.h>
#include <ArduinoTimer.h>
#include <CircularBuffer.h>
#include <EEPROMStore.h>
#include <Filter.h>

#include <Wire.h>
#include <HMC5883L.h>
#include <Servo.h>


// This is our I2C Compass
HMC5883L compass;

// This is our continuous rotation servo
Servo servo;

// Servo constants
int NORTH = 0;
int SERVO_STOP = 90;  // change this value to achieve minimum rotation!
int SERVO_PAN = 4;
int SERVO_TILT = 5;

// Pi for calculations - not the raspberry type
const float Pi = 3.14159;

// Current Target and Last Heading
static int target;
static int lasttarget;
// Other Data
float UAV_LAT = 0;
float UAV_LONG = 0;
float UAV_DISTANCE = 0;
float UAV_ELEVATION = 0;
float UAV_AZIMUTH = 0;
float UAV_BEARING = 0;

// Command Processor
// Attach a new CmdMessenger object to the default Serial port
CommandHandler<> SerialCommandHandler;

// Commands we send from the PC and want to receive on the Arduino.
// We must define a callback function in our Arduino program for each entry in the list below.
void attachCommandCallbacks()
{
  Serial.println("Attaching Callbacks");
  // Attach callback methods
  // Setup the serial commands we can repond to
  SerialCommandHandler.AddCommand(F("UAV"), OnUAV);

  SerialCommandHandler.SetDefaultHandler(Cmd_Unknown);

}

// ------------------  C A L L B A C K S -----------------------
void OnUAV(CommandParameter &Parameters)
{
    Serial.println("New Update: ");
  // !UAV 50.9100974 -1.5351514 15.0 -8.0 137.795275591
  // Retreive first parameter as float
  UAV_LAT = atof(Parameters.NextParameter());
    UAV_LONG = atof(Parameters.NextParameter());
  UAV_DISTANCE = atof(Parameters.NextParameter());
  UAV_AZIMUTH= Parameters.NextParameterAsInteger(UAV_AZIMUTH);
  UAV_BEARING= Parameters.NextParameterAsInteger(UAV_BEARING);
  Serial.print("Azimuths:");
  Serial.println(UAV_AZIMUTH);
  Serial.print("Bearing:");
  Serial.println(UAV_BEARING);
}

void Cmd_Unknown()
{
  Serial.println(F("I don't understand"));
}













// Arduino Setup Function
void setup(void) 
{
  Serial.begin(115200);

 // Initialize HMC5883L
  Serial.println("\nInitialize HMC5883L");
  while (!compass.begin())
  {
    Serial.println ( "HMC5883L not found, check connection!" ) ;
    delay(500);
  }

  Serial.println("Setting up HCM5883L");
  // Set the measuring range
  compass.setRange(HMC5883L_RANGE_1_3GA);
 
  // Set the operating mode
  compass.setMeasurementMode(HMC5883L_CONTINOUS);
 
  // Setting the measurement frequency
  compass.setDataRate(HMC5883L_DATARATE_15HZ);
 
  // The number of averaged samples
  compass.setSamples(HMC5883L_SAMPLES_4);

  // Attach Servo
  Serial.println("Connecting to servos");
  servo.attach(SERVO_PAN);  // attaches the servo on pin 9 to the servo object 
  servo.write(SERVO_STOP);  
  
  MoveServo(NORTH);

  attachCommandCallbacks();
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
   // Process incoming serial data, and perform callbacks
  SerialCommandHandler.Process();
  
  MoveServo(UAV_BEARING);

  delay(250);
}
