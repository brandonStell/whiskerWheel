#include <elapsedMillis.h>

#include <Encoder.h>
#include <elapsedMillis.h>
#include <AccelStepper.h>
#include <Adafruit_MotorShield.h>
elapsedMillis timeElapsed;



Encoder wheelEnc(3, 2);
Encoder motor1Enc(19, 16);
Encoder motor2Enc(18, 17);
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_StepperMotor *myStepper1 = AFMS.getStepper(200, 1);
Adafruit_StepperMotor *myStepper2 = AFMS.getStepper(200, 2);


void forwardstep1() {  
  myStepper1->onestep(FORWARD, DOUBLE);
}
void backwardstep1() {  
  myStepper1->onestep(BACKWARD, DOUBLE);
}

// wrappers for the second motor!
void forwardstep2() {  
  myStepper2->onestep(FORWARD, DOUBLE);
}
void backwardstep2() {  
  myStepper2->onestep(BACKWARD, DOUBLE);
}

AccelStepper stepper1(forwardstep1, backwardstep1);
AccelStepper stepper2(forwardstep2, backwardstep2);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);           // set up Serial library at 9600 bps

    
  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz

  stepper1.setMaxSpeed(2000.0);  // steps per second
  stepper1.setSpeed(1000.0);
  stepper1.setAcceleration(10.0);

  stepper2.setMaxSpeed(2000.0);  // steps per second
  stepper2.setSpeed(1000.0);
  stepper2.setAcceleration(10.0);
}

int sampleInterval = 20;  //sampling rate in ms
double encoderPositionsPerTurn = 8192;
const double motorStepsPerTurn = 200;
long oldWheelPosition  = -999;
long oldMotor1Position = -999;
long oldMotor2Position = -999;
double samplesPerSecond = 1000 / sampleInterval;
double lastWheelEncoderMove, lastMotor1EncoderMove, lastMotor2EncoderMove;
double motorStepsPerSecond;
long newWheelPosition = wheelEnc.read();
long newMotor1Position = motor1Enc.read();
long newMotor2Position = motor2Enc.read();
    
void loop() {
  newWheelPosition = wheelEnc.read();
  newMotor1Position = motor1Enc.read();
  newMotor2Position = motor2Enc.read();
  if(timeElapsed > sampleInterval){
    lastWheelEncoderMove = oldWheelPosition - newWheelPosition;
    motorStepsPerSecond = (motorStepsPerTurn * lastWheelEncoderMove * samplesPerSecond / encoderPositionsPerTurn);    // RPM
    oldWheelPosition = newWheelPosition;
    oldMotor1Position = newMotor1Position;
    oldMotor2Position = newMotor2Position;

    Serial.print(millis());
    Serial.print(" ");
    Serial.print(newWheelPosition);
    Serial.print(" ");
    Serial.print(newMotor1Position);
    Serial.print(" ");
    Serial.println(newMotor2Position);
    
    timeElapsed = 0;
  }
  if(motorStepsPerSecond != 0){
    stepper1.setSpeed(-motorStepsPerSecond);// steps per second
    stepper1.runSpeed();
    
    stepper2.setSpeed(motorStepsPerSecond);// steps per second
    stepper2.runSpeed();
  }
  else if(motorStepsPerSecond == 0.00){
    myStepper1->release();
    myStepper2->release();
  }
}

