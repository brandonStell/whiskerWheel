////////////////sets the stepper speed based on the POSITION of the encoder/////////////////
////////////////we're using a MEGA for its the extra interrupts.////////////////////////////

#include <Encoder.h>
#include <elapsedMillis.h>
#include <AccelStepper.h>
#include <Adafruit_MotorShield.h>

Encoder motor1Enc(2, 3);
//Encoder motor2Enc(20,21);
Encoder wheelEnc(18,19);
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_StepperMotor *myStepper1 = AFMS.getStepper(200, 2);
Adafruit_StepperMotor *myStepper2 = AFMS.getStepper(200, 1);

void forwardstep1() {  
  myStepper1->onestep(FORWARD, SINGLE);
}
void backwardstep1() {  
  myStepper1->onestep(BACKWARD, SINGLE);
}
// wrappers for the second motor!
void forwardstep2() {  
  myStepper2->onestep(FORWARD, SINGLE);
}
void backwardstep2() {  
  myStepper2->onestep(BACKWARD, SINGLE);
}

AccelStepper stepper1(forwardstep1, backwardstep1);
AccelStepper stepper2(forwardstep2, backwardstep2);

void setup()
{  
   Serial.begin(9600);           // set up Serial library at 9600 bps
   Serial.println("Stepper test!");
  
  AFMS.begin();  // create with the default frequency 1.6KHz

  stepper1.setMaxSpeed(1000.0);
  stepper1.setAcceleration(1000.0);
   
  stepper2.setMaxSpeed(1000.0);
  stepper2.setAcceleration(1000.0);    
}

long oldWheelPosition  = -999;
long oldMotor2Position = -999;

void loop() {
  long newWheelPosition = wheelEnc.read();
  long newMotor1Position = motor1Enc.read();
  while(newWheelPosition > newMotor1Position+40){
    stepper1.move(1000);
    stepper1.run();
    newWheelPosition = wheelEnc.read();
    newMotor1Position = motor1Enc.read();
    Serial.print(newWheelPosition);
    Serial.print(" ");
    Serial.println(newMotor1Position);
  }
  while(newWheelPosition < newMotor1Position-40){
    stepper1.move(-1000);
    stepper1.run();
    newWheelPosition = wheelEnc.read();
    newMotor1Position = motor1Enc.read();
    Serial.print(newWheelPosition);
    Serial.print(" ");
    Serial.println(newMotor1Position);
  }
  Serial.print(newWheelPosition);
  Serial.print(" ");
  Serial.println(newMotor1Position);
    stepper1.moveTo(stepper1.currentPosition());
    myStepper1->release();
}
