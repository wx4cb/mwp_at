#ifndef __STEPPER_H__
#define __STEPPER_H__
#include <AccelStepper.h>
#include <AFMotor.h>

#define STEPS_PER_REV 200
AF_Stepper afstepper_PanStepper(STEPS_PER_REV, 1);
AF_Stepper afstepper_TiltStepper(STEPS_PER_REV, 12);


extern int pan_servo_error;
extern int tilt_servo_error;
extern int pan_current;
extern int tilt_current;
void pan_backwardstep();
void pan_forwardstep();
float step_degree(float desired_degree);
int MoveStepper(int Heading);
int GetPanStepperPos(void);
void InitStepper(void);


float step_degree(float desired_degree) {
  Serial.print("Stepper Wants: ");
  Serial.print(desired_degree);
  return (desired_degree / STEPS_PER_REV);
}

/*
   you can change these to DOUBLE or INTERLEAVE or MICROSTEP!
*/
void pan_forwardstep() {
  afstepper_PanStepper.onestep(FORWARD, DOUBLE);
  pan_servo_error = GetPanStepperPos();
  //Serial.print("Current Servo Position: ");
  //Serial.println(pan_servo_error);
}
void pan_backwardstep() {
  afstepper_PanStepper.onestep(BACKWARD, DOUBLE);
  pan_servo_error = GetPanStepperPos();
  //Serial.print("Current Servo Position: ");
  //Serial.println(pan_servo_error);
}
void tilt_forwardstep() {
  afstepper_TiltStepper.onestep(FORWARD, SINGLE);

}
void tilt_backwardstep() {
  afstepper_TiltStepper.onestep(BACKWARD, SINGLE);
}
AccelStepper PanStepperObj(pan_forwardstep, pan_backwardstep); // use functions to step
AccelStepper TiltStepperObj(tilt_forwardstep, tilt_backwardstep); // use functions to step

int MovePanStepper(int Heading) {
  char buf[60];  
  if (GetPanStepperPos() > Heading) {
    PanStepperObj.setSpeed(-100);
    PanStepperObj.moveTo(Heading);
  } else {
    PanStepperObj.setSpeed(100);
    PanStepperObj.moveTo(Heading);
  }
  while (GetPanStepperPos() != 0) {
    // Process IMU Data
    GetIMU();
    sprintf(buf, "Pan Current: %d - Pan Target: %d - Error: %d\n", pan_current, pan_target, pan_servo_error);
    Serial.print(buf);
    PanStepperObj.runSpeedToPosition();
  }
}

int GetPanStepperPos(void) {
  return (PanStepperObj.distanceToGo());
}
void InitStepper(void) {
  Serial.println("Init Stepper");

  PanStepperObj.setMaxSpeed(200);
  PanStepperObj.setAcceleration(200.0);
  TiltStepperObj.setAcceleration(200.0);
}
#endif
