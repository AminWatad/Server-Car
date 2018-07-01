
#include "Motor.h"

using namespace std;

Motor::Motor() 
{
	//Create a GPIO object and set direction
	rightMotorGPIO = new GPIOClass("2");
	rightMotorGPIO->setdir_gpio("out");

	leftMotorGPIO = new GPIOClass("3");
	leftMotorGPIO->setdir_gpio("out");
	rightMotorGPIO->setval_gpio("1");
	leftMotorGPIO->setval_gpio("1");

}

Motor::~Motor()
{
	delete rightMotorGPIO;
	rightMotorGPIO = 0;

	delete leftMotorGPIO;
	rightMotorGPIO = 0;
}

void Motor::moveForward() 
{
	string leftMotorVal;
	leftMotorGPIO->getval_gpio(leftMotorVal);

	string rightMotorVal;
	rightMotorGPIO->getval_gpio(rightMotorVal);
	//Set the value of motorGPIO to 1. 
	rightMotorGPIO->setval_gpio("0");
	leftMotorGPIO->setval_gpio("0");
}

void Motor::stopMotor() 
{
	//Set the value of motorGPIO to 0.
	rightMotorGPIO->setval_gpio("1");
	leftMotorGPIO->setval_gpio("1");
}

void Motor::turnLeft() 
{
	//Get the initial states of the motors.
	string leftMotorVal; 
	leftMotorGPIO->getval_gpio(leftMotorVal);

	string rightMotorVal;
	rightMotorGPIO->getval_gpio(rightMotorVal);

	//Set the value of the left motor to 1,
	//and the right motor to 0;
	rightMotorGPIO->setval_gpio("0");
	leftMotorGPIO->setval_gpio("1");
	
	//Wait for the turn to complete.
	//usleep(5000);

	//Set back the motor to the previous state. 
	//rightMotorGPIO->setval_gpio(rightMotorVal);
	//leftMotorGPIO->setval_gpio(leftMotorVal);
}

void Motor::turnRight() 
{
	//Get the initial states of the motors.
	string leftMotorVal; 
   leftMotorGPIO->getval_gpio(leftMotorVal);

	string rightMotorVal; 
	rightMotorGPIO->getval_gpio(rightMotorVal);

	//Set the value of the left motor to 0,
	//and the right motor to 1.
	rightMotorGPIO->setval_gpio("1");
	leftMotorGPIO->setval_gpio("0");
	
	//Wait for the turn to complete.
	//usleep(5000);

	//Set back the motor to the previous state. 
	//rightMotorGPIO->setval_gpio(rightMotorVal);
	//leftMotorGPIO->setval_gpio(leftMotorVal);
}

