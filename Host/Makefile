# Makefile for Car-Command 

CXX = g++
MAIN = test.cpp
CXXFLAGS = -std=c++11 -Wall -g `pkg-config --cflags glib-2.0`
CLASSES = host.cpp Motor.cpp GPIOClass.cpp Car.cpp Client.cpp UART.cpp
TARGET = host
LIBS = `pkg-config --libs glib-2.0 gio-2.0`
OBJS := ${CLASSES:.cpp=.o}
FILES = $(TARGET) 

all:  $(FILES) 

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

clean:
	rm -f *.o $(TARGET)  
