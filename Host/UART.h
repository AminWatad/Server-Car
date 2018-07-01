#ifndef _UART_H_
#define _UART_H_

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <iostream>
#include "math.h"
using namespace std;
/* The purpose of this class is to communicate 
 * With the AVR. The main function of it is to
 * Recieve data, and populate the buffer
 * Enable UART in /boot/config.txt
 */
class UART 
{
public:
	UART();
	~UART();
	bool dataRecieve();
	int getData();
	void initUART();
	bool dataSend(unsigned char);
private:
	unsigned char buffer[256];
	int uart_filestream;
	int data;
};
#endif
