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

// wrappers for the motors
void forwardstep1() {  
  myStepper1->onestep(FORWARD, DOUBLE);
}
void backwardstep1() {  
  myStepper1->onestep(BACKWARD, DOUBLE);
}
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

  stepper1.setMaxSpeed(1000.0);  // steps per second
  stepper1.setAcceleration(1000.0);
  stepper2.setMaxSpeed(1000.0);  // steps per second
  stepper2.setAcceleration(1000.0);
}

int sampleInterval = 5;  //sampling rate in ms
double encoderPositionsPerTurn = 8192;
const double motorStepsPerTurn = 200;
long oldWheelPosition  = -999;
long oldMotor1Position = -999;
long oldMotor2Position = -999;
double samplesPerSecond = 1000 / sampleInterval;
long newWheelPosition = wheelEnc.read(), newMotor1Position = motor1Enc.read(), newMotor2Position = motor2Enc.read();
double lastWheelEncoderMove, lastMotor1Move, lastMotor2Move;
double motorStepsPerSecond;
double gain = 1;
boolean speeds = false;
boolean constrainPosition = true;
double motor1Speed, motor2Speed, wheelSpeed;


void loop() {
  newWheelPosition = wheelEnc.read(), newMotor1Position = motor1Enc.read(), newMotor2Position = motor2Enc.read();
  
  if(timeElapsed > sampleInterval){
    lastWheelEncoderMove = oldWheelPosition - newWheelPosition;
    lastMotor1Move = oldMotor1Position - newMotor1Position;
    lastMotor2Move = oldMotor2Position - newMotor2Position;
    motorStepsPerSecond = (motorStepsPerTurn * lastWheelEncoderMove * samplesPerSecond / encoderPositionsPerTurn / gain);    // RPM
    if(motorStepsPerSecond == 0){
      stepper1.moveTo(stepper1.currentPosition());
      myStepper1->release();
      
      stepper2.moveTo(stepper2.currentPosition());
      myStepper2->release();
    }
    else if (motorStepsPerSecond > 0){
      stepper1.setSpeed(motorStepsPerSecond);// steps per second
      stepper1.move(-10000);
      
      stepper2.setSpeed(motorStepsPerSecond);// steps per second
      stepper2.move(10000);
    }
    else if (motorStepsPerSecond < 0){
      stepper1.setSpeed(motorStepsPerSecond);// steps per second
      stepper1.move(10000);
      
      stepper2.setSpeed(motorStepsPerSecond);// steps per second
      stepper2.move(-10000);
    }
    
    wheelSpeed = (lastWheelEncoderMove/ encoderPositionsPerTurn * samplesPerSecond * 62.83);
    motor1Speed = (lastMotor1Move / encoderPositionsPerTurn * samplesPerSecond * 62.83);
    motor2Speed = (lastMotor2Move/ encoderPositionsPerTurn * samplesPerSecond * 62.83);
    SerialSend();
    
    oldWheelPosition = newWheelPosition;
    oldMotor1Position = newMotor1Position;
    oldMotor2Position = newMotor2Position;
    timeElapsed = 0;
  }

  if (constrainPosition){
    while (newWheelPosition > newMotor2Position+80){
      stepper1.run();
      stepper2.run();
      newWheelPosition = wheelEnc.read();
      newMotor1Position = motor1Enc.read();
      newMotor2Position = motor2Enc.read();
      SerialSend();
    }
    while (newWheelPosition < newMotor2Position-80){
      stepper1.run();
      stepper2.run();
      newWheelPosition = wheelEnc.read();
      newMotor2Position = motor1Enc.read();
      newMotor2Position = motor2Enc.read();
      SerialSend();
    }
  }
}

void SerialSend()
{
  if(!speeds){
    Serial.print("motor1: ");Serial.print(newMotor1Position); Serial.print(" ");
    Serial.print("motor2: ");Serial.print(newMotor2Position); Serial.print(" ");
    Serial.print("wheel: ");Serial.print(newWheelPosition); Serial.println(" ");
  } else {
    Serial.println("speed output");
    Serial.print("motor1: ");Serial.print(motor1Speed); Serial.print(" ");
    Serial.print("motor2: ");Serial.print(motor2Speed); Serial.print(" ");
    Serial.print("wheel: ");Serial.print(wheelSpeed); Serial.println();
  }
}

void SerialReceive()
{
  if(Serial.available()){
   char b = Serial.read(); 
   Serial.flush(); 
   if(b=='9'){
    gain - 0.2;
   }
   if(b=='0'){
    gain + 0.2;
   }
   if(b=='1'){
    speeds=true;
   }
   if(b=='2'){
    speeds=false;
   }
  }
}
