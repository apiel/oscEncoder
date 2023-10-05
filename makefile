LIBLO=`pkg-config --cflags --libs liblo`

ifneq ($(shell uname -m),x86_64)
GPIO := -lpigpio -lrt -DPIGPIO=1
endif

all: build run

build:
	g++ -o oscEncoder main.cpp $(LIBLO) $(GPIO)

tester:
	g++ -o encoderTester encoderTester.cpp $(GPIO)

run:
	./oscEncoder
