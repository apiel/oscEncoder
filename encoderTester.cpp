#ifdef PIGPIO
// sudo apt-get install libpigpio-dev
#include <pigpio.h>
#endif

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <cstdlib>

class RotaryEncoder
{
protected:
    int levelA = 0;
    int levelB = 0;
    int lastGpio = -1;

    static void pulseEx(int gpio, int level, uint32_t tick, void *user)
    {
        RotaryEncoder *self = (RotaryEncoder *)user;
        self->pulse(gpio, level, tick);
    }

    void send(uint8_t direction)
    {
        printf("[encoderId %d] send: %d\n", encoderId, direction);
    }

public:
    int gpioA;
    int gpioB;
    uint8_t encoderId;

    RotaryEncoder(int _gpioA, int _gpioB, uint8_t _encoderId)
        : gpioA(_gpioA), gpioB(_gpioB), encoderId(_encoderId)
    {
        printf("gpioA: %d, gpioB: %d, encoderId: %d\n", gpioA, gpioB, encoderId);

#ifdef PIGPIO
        gpioSetMode(gpioA, PI_INPUT);
        gpioSetMode(gpioB, PI_INPUT);
        gpioSetPullUpDown(gpioA, PI_PUD_UP);
        gpioSetPullUpDown(gpioB, PI_PUD_UP);
        gpioSetAlertFuncEx(gpioA, pulseEx, this);
        gpioSetAlertFuncEx(gpioB, pulseEx, this);
#endif
    }

    void pulse(int gpio, int level, uint32_t tick)
    {
        if (gpio == gpioA)
        {
            levelA = level;
        }
        else
        {
            levelB = level;
        }

        if (gpio != lastGpio) /* debounce */
        {
            lastGpio = gpio;

            if ((gpio == gpioA) && (level == 1))
            {
                if (levelB)
                {
                    send(-1);
                }
            }
            else if ((gpio == gpioB) && (level == 1))
            {
                if (levelA)
                {
                    send(1);
                }
            }
        }
    }
};

// get argc
// osctester gpioA gpioB
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

    if (argc != 3) {
        printf("Usage: %s gpioA gpioB\n", argv[0]);
        return 1;
    }

    int gpioA = atoi(argv[1]);
    int gpioB = atoi(argv[2]);


    RotaryEncoder encoder = RotaryEncoder(gpioA, gpioB, 0);

    while (1)
    {
        usleep(1000);
    }

    return 0;
}

// g++ -std=c++11 main.cpp
