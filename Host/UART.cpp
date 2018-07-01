#include "UART.h"

UART::UART()
{
	uart_filestream = -1;
}

UART::~UART()
{	
	close(uart_filestream);
}

void UART::initUART() 
{
	uart_filestream = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY);
	if(uart_filestream == -1)
	{
		cout << "ERROR - UNABLE TO OPEN UART" << endl;
	}

	struct termios options;
	tcgetattr(uart_filestream, &options);
	options.c_cflag = B38400 | CS8 | CLOCAL | CREAD;
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(uart_filestream, TCIFLUSH);
	tcsetattr(uart_filestream, TCSANOW, &options);
}

bool UART::dataRecieve() 
{
	int length = read(uart_filestream, (void*) buffer, 255);

	if (length < 0) {}
	//No new input, ignore it. 
	else if (length == 0) {}
	else {
		buffer[length] = '\0';
		data = (int)buffer[0];
		return true;
	}
	return false;
}

bool UART::dataSend(unsigned char val)
{	unsigned char buf[20];
	buf[0] = val;
	int count = write(uart_filestream, &buf, 1);
	if (count < 0)
	{
		return false;
	}
	
	return true;
}
int UART::getData()
{	
	return data;
}


