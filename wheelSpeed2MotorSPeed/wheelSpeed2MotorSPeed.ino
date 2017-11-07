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

  stepper2.setMaxSpeed(1000.0);  // steps per second
  stepper2.setAcceleration(1000.0);
//  stepper2.move(10000);
}

int sampleInterval = 5;  //sampling rate in ms
double encoderPositionsPerTurn = 8192;
const double motorStepsPerTurn = 200;
long oldWheelPosition  = -999;
long oldMotor2Position = -999;
double samplesPerSecond = 1000 / sampleInterval;

void loop() {
  long newWheelPosition = wheelEnc.read();
  long newMotor2Position = motor2Enc.read();
  if(timeElapsed > sampleInterval){
    double lastWheelEncoderMove = oldWheelPosition - newWheelPosition;
    double motorStepsPerSecond = (motorStepsPerTurn * lastWheelEncoderMove * samplesPerSecond / encoderPositionsPerTurn);    // RPM
    if(motorStepsPerSecond == 0){
      stepper2.moveTo(stepper2.currentPosition());
      myStepper2->release();
    }
    else if (motorStepsPerSecond > 0 && motorStepsPerSecond < 200){
      stepper2.setSpeed(motorStepsPerSecond);// steps per second
      stepper2.move(10000);
    }
    else if (motorStepsPerSecond < 0 && motorStepsPerSecond > -200){
      stepper2.setSpeed(motorStepsPerSecond);// steps per second
      stepper2.move(-10000);
    }
    Serial.print(newWheelPosition);
    Serial.print(" ");
    Serial.println(newMotor2Position);
//    Serial.print(" ");
//    Serial.println(lastWheelEncoderMove/ encoderPositionsPerTurn * samplesPerSecond * 62.83);
    oldWheelPosition = newWheelPosition;
    timeElapsed = 0;
  }
  while (newWheelPosition > newMotor2Position+80){
    stepper2.run();
    Serial.print(newWheelPosition);
    Serial.print(" ");
    Serial.println(newMotor2Position);
//    Serial.print(" ");
//    Serial.println("---");
    newWheelPosition = wheelEnc.read();
    newMotor2Position = motor2Enc.read();
  }
  while (newWheelPosition < newMotor2Position-80){
    stepper2.run();
    Serial.print(newWheelPosition);
    Serial.print(" ");
    Serial.println(newMotor2Position);
//    Serial.print(" ");
//    Serial.println(" ---");
    newWheelPosition = wheelEnc.read();
    newMotor2Position = motor2Enc.read();
  }
}
