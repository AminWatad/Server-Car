#ifndef MOTOR_H_
#define MOTOR_H_

#include "GPIOClass.h"
#include <string>
#include <iostream>

class GPIOClass;

using namespace std;
/* Motor Class
 * The purpose of this class is to control the motors.
 * Uses the GPIOClass to control the GPIOs.
 * The constructor chooses the next free GPIO. 
 */
class Motor 
{
public:
	Motor();
	~Motor();
	void moveForward();
	void stopMotor();
	void turnLeft();
	void turnRight();
private:
	GPIOClass* rightMotorGPIO;;
	GPIOClass* leftMotorGPIO;
};

#endif
