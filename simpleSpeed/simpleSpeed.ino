#include <elapsedMillis.h>

#include <Encoder.h>
#include <elapsedMillis.h>
#include <AccelStepper.h>
#include <Adafruit_MotorShield.h>
elapsedMillis timeElapsed;



Encoder wheelEnc(3, 2);
Encoder motor2Enc(18, 17);
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_StepperMotor *myStepper2 = AFMS.getStepper(200, 2);

// wrappers for the second motor!
void forwardstep2() {  
  myStepper2->onestep(FORWARD, DOUBLE);
}
void backwardstep2() {  
  myStepper2->onestep(BACKWARD, DOUBLE);
}

AccelStepper stepper2(forwardstep2, backwardstep2);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);           // set up Serial library at 9600 bps

    
  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz

  stepper2.setMaxSpeed(2000.0);  // steps per second
  stepper2.setSpeed(1000.0);
  stepper2.setAcceleration(10.0);
}

int sampleInterval = 5;  //sampling rate in ms
double encoderPositionsPerTurn = 8192;
const double motorStepsPerTurn = 200;
long oldWheelPosition  = -999;
long oldMotor2Position = -999;
double samplesPerSecond = 1000 / sampleInterval;
double lastWheelEncoderMove, lastMotor1EncoderMove, lastMotor2EncoderMove;
double motorStepsPerSecond;
    
void loop() {
  long newWheelPosition = wheelEnc.read();
  long newMotor2Position = motor2Enc.read();
  if(timeElapsed > sampleInterval){
    lastWheelEncoderMove = oldWheelPosition - newWheelPosition;
    lastMotor2EncoderMove = oldMotor2Position - newMotor2Position;
    motorStepsPerSecond = (motorStepsPerTurn * lastWheelEncoderMove * samplesPerSecond / encoderPositionsPerTurn);    // RPM
    Serial.println(motorStepsPerSecond);
    oldWheelPosition = newWheelPosition;
    oldMotor2Position = newMotor2Position;
    
    timeElapsed = 0;
  }
if(motorStepsPerSecond != 0){
  stepper2.setSpeed(motorStepsPerSecond);// steps per second
  stepper2.runSpeed();
}
else if(motorStepsPerSecond == 0.00){
   myStepper2->release();
}


}

