#ifdef PIGPIO
// sudo apt-get install libpigpio-dev
#include <pigpio.h>
#endif

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <cstdlib>

class Button
{
protected:
    static void pulseEx(int gpio, int level, uint32_t tick, void *user)
    {
        Button *self = (Button *)user;
        self->pulse(gpio, level, tick);
    }

    void send(uint8_t direction)
    {
        printf("[buttomId %d] send: %d\n", buttomId, direction);
    }

public:
    int gpio;
    uint8_t buttomId;

    Button(int _gpio, uint8_t _buttomId)
        : gpio(_gpio), buttomId(_buttomId)
    {
        printf("gpio: %d, buttomId: %d\n", gpio, buttomId);

#ifdef PIGPIO
        gpioSetMode(gpio, PI_INPUT);
        gpioSetPullUpDown(gpio, PI_PUD_UP);
        gpioSetAlertFuncEx(gpio, pulseEx, this);
#endif
    }

    void pulse(int gpio, int level, uint32_t tick)
    {
        printf("gpio: %d, level: %d, tick: %d\n", gpio, level, tick);
    }
};

// get argc
// osctester gpio gpioB
int main(int argc, char **argv)
{
    printf("Start OSC encoder.\n");

#ifdef PIGPIO
    if (gpioInitialise() < 0)
    {
        printf("Failed to initialise GPIO\n");
        return 1;
    }
#endif

    if (argc != 2) {
        printf("Usage: %s gpio\n", argv[0]);
        return 1;
    }

    int gpio = atoi(argv[1]);


    Button encoder = Button(gpio, 0);

    while (1)
    {
        usleep(1000);
    }

    return 0;
}

// g++ -std=c++11 main.cpp
