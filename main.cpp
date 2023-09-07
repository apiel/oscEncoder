#ifdef PIGPIO
// sudo apt-get install libpigpio-dev
#include <pigpio.h>
#endif

#include <lo/lo.h>

#include <stdint.h>
#include <vector>
#include <stdio.h>
#include <unistd.h>

#define ENCODER_COUNT 4

lo_address oscClient = lo_address_new(NULL, "8888");

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
        // printf("[encoderId %d] send: %d\n", encoderId, direction);
        lo_send(oscClient, "/encoder", "ii", encoderId, direction);
    }

public:
    // void (*midiHandler)(std::vector<unsigned char>* message) = NULL;
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
                    send(1);
                }
            }
            else if ((gpio == gpioB) && (level == 1))
            {
                if (levelA)
                {
                    send(-1);
                }
            }
        }
    }
};

int main()
{
    printf("Start OSC encoder.\n");

#ifdef PIGPIO
    if (gpioInitialise() < 0)
    {
        printf("Failed to initialise GPIO\n");
        return 1;
    }
#endif

    if (!oscClient)
    {
        printf("Failed to create OSC client\n");
        return 1;
    }
    printf("Initialized OSC client on port 8888\n");

    RotaryEncoder encoders[ENCODER_COUNT] = {
        {4, 27, 0},
        {25, 24, 1},
        {19, 16, 2},
        {21, 20, 3}};

    while (1)
    {
        usleep(1000);
    }

    return 0;
}

// g++ -std=c++11 main.cpp
