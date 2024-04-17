#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/events/layer_state_changed.h>

#include "led.h"

#define LED_NODE_R DT_ALIAS(ledred)
#define LED_NODE_B DT_ALIAS(ledblue)

#if !DT_NODE_HAS_STATUS(LED_NODE_R, okay)
#if !DT_NODE_HAS_STATUS(LED_NODE_B, okay)
#error "Unsupported board: led devicetree alias is not defined"
#endif
#endif

static const struct gpio_dt_spec LED_R = GPIO_DT_SPEC_GET(LED_NODE_R, gpios);
static const struct gpio_dt_spec LED_B = GPIO_DT_SPEC_GET(LED_NODE_B, gpios);

void reset_leds() {
    if (!device_is_ready(LED_R.port)) {
        return;
    } else {
        gpio_pin_configure_dt(&LED_R, GPIO_DISCONNECTED);
    }

    if (!device_is_ready(LED_B.port)) {
        return;
    } else {
        gpio_pin_configure_dt(&LED_B, GPIO_DISCONNECTED);
    }
}

void set_led(size_t index, int state) {
    if(index == RED) {
        if (!device_is_ready(LED_R.port)) {
            return;
        }
        gpio_pin_configure_dt(&LED_R, state);
    } else if(index == BLUE) {
        if (!device_is_ready(LED_B.port)) {
            return;
        }
        gpio_pin_configure_dt(&LED_B, state);
    }
    return;
}