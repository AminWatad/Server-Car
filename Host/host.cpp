#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "Motor.h"
#include "Client.h"
#include "Car.h"
#include <csignal>

using namespace std;

void signal_handler(int sig);

bool signalRecieved = false;
Client* client = new Client("10.0.1.3", 7777);
GThread* threadRecieve;
GThread* threadSend;
Car* car = new Car();
static gpointer reciever(gpointer data) 
{
	char buffer[256];
	int size = 0;
	GInputStream *in = client->getInStream();
	while(0 < ( size = g_input_stream_read(in, buffer, sizeof buffer, NULL,
	                                                   NULL)))
	{
		client->command = g_strdup_printf("%s", buffer);
		client->newCom = true;
		for(int i = 0; i < (sizeof buffer); ++i) {
		   buffer[i] = 0;
		}
	}
}

static gpointer sender(gpointer data)
{
	string info;
	for(;;) {
		int mode = car->getMode();
		car->checkDistance();
	
		info = "";
		info = to_string(mode) + ";";
		info += to_string(car->getDistance()) + ";";
		info += "0;";
		info += "nothing;";
		client->sendInfo(info);
	
		sleep(1);
		}
}

int main (void) 
{
	signal(SIGINT, signal_handler);

	Motor* motor = new Motor();
	string mode = "";
	client->startCon();
	threadRecieve = g_thread_new("RECIEVER", reciever, NULL);	
	threadSend = g_thread_new("SENDER", sender, NULL);

	while(1) {
		string str; 	
		usleep(200);	
			
		if(client->newCommand())
		{
			str = string(client->command);
			car->sendMove(str);
			client->newCom = false;
		}


		if(signalRecieved) {
			delete motor;
			motor = 0;

			client = 0;
			
			g_thread_unref(threadRecieve);
			g_thread_unref(threadSend);

			delete car;
			car = 0;
			
			break;
			exit(0);
		}
	}
	cout << "Goodbye.." << endl;

	return 0;
}

void signal_handler(int sig)
{
	write(0,"Signal to kill the program has been recieved\n",46);
	signalRecieved = true;
}
