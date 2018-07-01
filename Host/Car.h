#ifndef CAR_H_
#define CAR_H_

#include "GPIOClass.h"
#include "UART.h"
#include <string>
#include <iostream>
#include <vector>
#include "math.h"

class GPIOClass;
class UART;

using namespace std;
/* This class has all of the information
 * the car. Such as Mode, speed, distance. 
 * Also responisble for reading it from GPIO.
 */

class Car
{
public:
	Car();
	~Car();
	int getMode();
	void checkMode();
	int getDistance();
	void checkDistance();
	bool sendMove(string);
private:
	int mode;
	int distance;
	GPIOClass* modePin1;
	GPIOClass* modePin2;
	UART* distUART;
};

#endif
