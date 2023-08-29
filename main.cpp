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
        // std::vector<unsigned char> message;
        // message.push_back(0xB0 + channel);
        // message.push_back(cc);
        // message.push_back(direction);
        // midiHandler(&message);
        printf("[cc %d] send: %d\n", cc, direction);
    }

public:
    // void (*midiHandler)(std::vector<unsigned char>* message) = NULL;
    int gpioA;
    int gpioB;
    uint8_t cc;
    uint8_t channel;

    RotaryEncoder(int _gpioA, int _gpioB, uint8_t _cc, uint8_t _channel)
        : gpioA(_gpioA), gpioB(_gpioB), cc(_cc), channel(_channel)
    {
        printf("gpioA: %d, gpioB: %d, cc: %d, channel: %d\n", gpioA, gpioB, cc, channel);

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
                    send(1); // 1
                }
            }
            else if ((gpio == gpioB) && (level == 1))
            {
                if (levelA)
                {
                    send(127); // -1
                }
            }
        }
    }
};

int main()
{
    printf("Start OSC encoder.\n");

    RotaryEncoder encoders[ENCODER_COUNT] = {
        RotaryEncoder(4, 27, 1, 80),
        RotaryEncoder(25, 24, 1, 81),
        RotaryEncoder(19, 16, 1, 82),
        RotaryEncoder(21, 20, 1, 83)};

    while (1)
    {
        usleep(1000);
    }
    

    return 0;
}

// g++ -std=c++11 main.cpp
