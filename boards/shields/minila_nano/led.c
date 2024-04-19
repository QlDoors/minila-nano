#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
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

void set_led_dt(const struct gpio_dt_spec* spec, enum LED_SWITCH sw)
{
    if (!device_is_ready(spec->port)) {
        LOG_ERR("LED %s is not ready.", spec->port->name);
        return;
    }

    int state = 0;
    switch (sw) {
    case CLOSE:
        state = GPIO_DISCONNECTED;
        break;

    case OPEN:
        if (spec->dt_flags == GPIO_ACTIVE_HIGH) {
            state = GPIO_OUTPUT_HIGH;
        } else if (spec->dt_flags == GPIO_ACTIVE_LOW) {
            state = GPIO_OUTPUT_LOW;
        } else {
            LOG_ERR("LED %s, flag %d, doesn't have property ACTIVE.", spec->port->name, spec->dt_flags);
        }
        break;

    default:
        LOG_ERR("set_led_dt(): sw %d is illegal.", sw);
        return;
    }
    gpio_pin_configure_dt(spec, state);
}

void reset_leds()
{
    set_led_dt(&LED_R, CLOSE);
    set_led_dt(&LED_B, CLOSE);
}

void set_led(size_t index, enum LED_SWITCH sw)
{
    if (index == RED) {
        set_led_dt(&LED_R, sw);
    } else if (index == BLUE) {
        set_led_dt(&LED_B, sw);
    }
    return;
}