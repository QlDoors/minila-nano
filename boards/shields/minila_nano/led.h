#include <zephyr/kernel.h>

#define RED 0
#define BLUE 1

enum LED_SWITCH {
    OPEN,
    CLOSE
};

void reset_leds();

void set_led(size_t index, enum LED_SWITCH sw);