#include <zephyr/kernel.h>

#define RED 0
#define BLUE 1

void reset_leds();

void set_led(size_t index, int state);