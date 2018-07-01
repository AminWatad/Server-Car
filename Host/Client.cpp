#include "Client.h"

Client::Client(string IP, int p) 
{
	ip = IP;
	port = p;
	command = (char*)"STOP";
	newCom = false;

	return;
}

Client::~Client() 
{
	delete connection;
	connection = 0;

	delete client;
	client = 0;

	delete in;
	in = 0;

	delete out;
	out = 0;
}

bool Client::startCon()
{
	char buffer[1024];

	client = g_socket_client_new();
	
	connection = g_socket_client_connect_to_host(client,
									(gchar*)ip.c_str(), port, NULL, &error);

	if (error != NULL) 
	{	
		g_error (error->message);
		return false;
	}
	else g_print("Connection successful!\n");

	in = 
	g_io_stream_get_input_stream(G_IO_STREAM(connection));

	out = 
	g_io_stream_get_output_stream(G_IO_STREAM(connection));
	
	g_input_stream_read(in, buffer, sizeof buffer, NULL, NULL);
	g_print(buffer);
	
	return true;
}

bool Client::sendInfo(string info) 
{
	char* str;

	str = g_strdup_printf("%s", info.c_str());
	g_output_stream_write(out, str, info.size(), NULL, &error);

	if (error != NULL)
	{
		g_error(error->message);
		return false;
	}
	
	return true;
}

GInputStream* Client::getInStream() {return in;}

GOutputStream* Client::getOutStream() {return out;}

bool Client::newCommand() {return newCom;}

void Client::beginRecieving() 
{
//	g_thread_new("RECIEVER", reciever, this);
}
