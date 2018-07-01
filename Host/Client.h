#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <gio/gio.h>
#include <glib.h>
#include <string>
#include <iostream>

using namespace std;
/* This class is responsible for handling
 * Server-Client functions. It handles 
 * Sending and receiving information
 * to and from the Server. 
 */
class Client
{
public: 
	Client(string, int);
	~Client();
	bool startCon();
	bool sendInfo(string);
	GInputStream* getInStream();
	GOutputStream* getOutStream();
	char * command;
	bool newCom;
	bool newCommand();
	void beginRecieving();
private:
	GSocketConnection *connection;
	GSocketClient *client;
	GInputStream *in;
	GOutputStream *out;
	int port;
	string ip;
	GError* error;
};
#endif
