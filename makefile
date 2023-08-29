LIBLO=`pkg-config --cflags --libs liblo`

GPIO=-lpigpio -lrt -DPIGPIO=1

all: build run

build:
	g++ -o oscEncoder main.cpp $(LIBLO) $(GPIO)

run:
	./oscEncoder