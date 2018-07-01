# Server Car

## Hardware
* Raspberry Pi 3
* Atmega 1284
Connected to the RPi3 using the GPio Pins. They communicate through USART. 
* Mac as a Server
* 4-DC Motors
* HC-SR04 Ultrasonic Sensor
* Motor Driver


## Software 
* Server Application 
This part runs on the Mac. Uses the gtk libarary to build the UI, and to run multithreaded process. (UI and Networking)
Written in C++, and uses python scripts to handle voice recognittion, and voice recroding.
Acts as the remote control for the Car. 

* Atmega Software
Runs on the Atmega 1284 chip. Handles multiple timers for task mangament, ultrasonic distance measuring. 
Handles USART communications as well. 

* Host Software
Runs on the raspberry pi. Handles network communications between host and server. Sends data to the Atmega through USART. 
GTK is used for multithreaded programming. 

## Functionality 
 * Mode 1
 The user controls the car using the arrows in the UI Application. 
 
 * Mode 2
 The user uses voice to control the car. Which runs a python script to record and analyze the file. 
 
 * Mode 3
 The part depends only on the Atmega Chip, Which makes the car move and avoiding obstacles on its own. No input needed. 
 
 * Distance
 Distance to the next obstacle is displayed in the UI Application. It is constantly updated every 300 ms. 
 
 * Netwrok
 The car connects to the Server through an IP and a port Number. Server and Host do not need to be on the same netwrok to connect. 
 
## Known Bugs and Shortcomings

  * The Ultrasonic could give wrong readings for far away distances, so it has been limited to read up to 400 cm.  
  * The Ultrasonic sensor fails to detect very thin, or low objects, which causes the car to keep going and crash. 
  
## Future work
 * Show the speed of the car on the application. The speed can be implemented by using accelerometer, or a speed decoder. 
 * Add the bluetooth functionality, so the user could also control the car through a smartphone. 
 * Implement a PWM, to control the speed of the car. 

## Youtube Link
  https://youtu.be/KaUg2c2xT-w


