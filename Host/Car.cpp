#include "Car.h"

Car::Car() 
{
	mode = 0;
	distance = 0;

	modePin1 = new GPIOClass("23");
	modePin1->setdir_gpio("in");

	modePin2 = new GPIOClass("24");
	modePin2->setdir_gpio("in");

	distUART = new UART();
	distUART->initUART();
	
}

Car::~Car() 
{
	delete modePin1;
	modePin1 = 0;

	delete modePin2;
	modePin2 = 0;

	delete distUART;
	distUART = 0;

}

int Car::getMode() {return mode;}

void Car::checkMode() 
{
	string pin1, pin2;

	modePin1->getval_gpio(pin1);
	modePin2->getval_gpio(pin2);

	if ((pin1 == "0") && (pin2 == "1")) mode = 1;
	else if ((pin1 == "1") && (pin2 == "0")) mode = 2;
	else if ((pin1 == "1") && (pin2 == "1")) mode = 3;
	else mode = 3;

	return;
}

int Car::getDistance() {return distance;}

void Car::checkDistance() 
{
	if(distUART->dataRecieve()) 
	{
		distance = distUART->getData();
	}

}

bool Car::sendMove(string move)
{
	if(move == "FORWARD" && mode == 3) { distUART->dataSend('3');}
	else if (move == "LEFT" && mode == 3) { distUART->dataSend('2');}
	else if (move == "RIGHT" && mode == 3) { distUART->dataSend('1');}
	else if (move == "STOP" && mode == 3) { distUART->dataSend('0');}
	else if (move == "BACKWARD" && mode == 3) { distUART->dataSend('4');}
	else if (move == "MODE1") { distUART->dataSend('5'); mode = 1;}
	else if (move == "MODE2") { distUART->dataSend('6'); mode = 2;}
	else if (move == "MODE3") { distUART->dataSend('7'); mode = 3;}
	else if (move == "COMFORWARD" && mode == 2) {
		distUART->dataSend('3');
	}
	else if (move == "COMLEFT" && mode == 2) {
	   distUART->dataSend('2');
	}
	else if (move == "COMRIGHT" && mode == 2) {
	   distUART->dataSend('1');
	}
	else if (move == "COMBACKWARD" && mode == 2) {
		distUART->dataSend('4');
	}
	else if (move == "COMSTOP" && mode == 2) {
		distUART->dataSend('0');
	}
	else if(move == "STARTAUTO" && mode == 1) distUART->dataSend('3');
	else if(move == "STOPAUTO" && mode == 1) distUART->dataSend('0');
	return true;
}
