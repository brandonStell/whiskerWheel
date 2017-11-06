#include <Encoder.h>
#include <elapsedMillis.h>
#include <AccelStepper.h>
#include <Adafruit_MotorShield.h>
elapsedMillis timeElapsed;



Encoder wheelEnc(2, 3);
Encoder motor2Enc(19, 18);
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_StepperMotor *myStepper2 = AFMS.getStepper(200, 1);

// wrappers for the second motor!
void forwardstep2() {  
  myStepper2->onestep(FORWARD, SINGLE);
}
void backwardstep2() {  
  myStepper2->onestep(BACKWARD, SINGLE);
}

AccelStepper stepper2(forwardstep2, backwardstep2);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);           // set up Serial library at 9600 bps

    
  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz

  stepper2.setMaxSpeed(1000.0);  // steps per second
  stepper2.setAcceleration(1000.0);
  stepper2.move(10000);

}

long lastMotor2Pos = stepper2.currentPosition();
unsigned long lastStepTime = micros();
const int sampleInterval = 10;  //sampling rate in ms
const int encoderPositionsPerTurn = 8192;
const int motorStepsPerTurn = 200;
const int encoderPosPerMotorStep = encoderPositionsPerTurn / motorStepsPerTurn;
const float wheelCircumference = 62.83; // 20cm * pi
const float cmPerEncoderTick = wheelCircumference / encoderPositionsPerTurn;
const float samplesPerSecond = 1000 / sampleInterval;
const float samplesPerMinute = samplesPerSecond * 60;
const float cmPerMotorTick = wheelCircumference / motorStepsPerTurn;
const float stepsPerSecondPerTicks = motorStepsPerTurn * samplesPerSecond / encoderPositionsPerTurn;  // = steps / (seconds * encoderticks)
long oldWheelPosition  = -999;
long oldMotor2Position = -999;

void loop() {
  long newWheelPosition = wheelEnc.read();
  long newMotor2Position = motor2Enc.read();
  if(timeElapsed > sampleInterval){
    int lastWheelEncoderStep = abs(oldWheelPosition - newWheelPosition);
    int lastMotor2EncoderStep = abs(oldMotor2Position - newMotor2Position);
    int lastWheelEncoderMove = oldWheelPosition - newWheelPosition;
    float wheelSpeed = (cmPerEncoderTick * lastWheelEncoderMove) * samplesPerSecond;           // cm/s
    float previousRotation = lastWheelEncoderMove / encoderPositionsPerTurn;
    float motorStepsPerSecond = (stepsPerSecondPerTicks * lastWheelEncoderMove);    // RPM

    if(newMotor2Position == newWheelPosition){
      stepper2.moveTo(stepper2.currentPosition());
      myStepper2->release();
    }
   while(newWheelPosition < newMotor2Position){
      stepper2.move(200);
      stepper2.run();
      newWheelPosition = wheelEnc.read();
      newMotor2Position = motor2Enc.read();
    }
    while(newWheelPosition > newMotor2Position){
      stepper2.move(-200);
      stepper2.run();
      newWheelPosition = wheelEnc.read();
      newMotor2Position = motor2Enc.read();
    }
    
    //      Serial.print((lastMotor2EncoderStep *  cmPerEncoderTick) / (timeElapsed / 1e3));
      Serial.print(motorStepsPerSecond);
      Serial.print(" ");
//      Serial.println((lastWheelEncoderStep *  cmPerEncoderTick) / (timeElapsed / 1e3));
      Serial.println(newMotor2Position);
    oldWheelPosition = newWheelPosition;
    oldMotor2Position = newMotor2Position;
    stepper2.setSpeed(motorStepsPerSecond);// steps per second
    timeElapsed = 0;
  }
  stepper2.run();
}
