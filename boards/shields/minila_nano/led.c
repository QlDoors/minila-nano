#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <drivers/ext_power.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(minila_led, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/events/layer_state_changed.h>

#include "led.h"

#define LED_NODE_R DT_ALIAS(ledred)
#define LED_NODE_B DT_ALIAS(ledblue)

#if !DT_NODE_HAS_STATUS(LED_NODE_R, okay)
#if !DT_NODE_HAS_STATUS(LED_NODE_B, okay)
#error "Unsupported board: led devicetree alias is not defined"
#endif
#endif

static const struct device* const ext_power = DEVICE_DT_GET(DT_INST(0, zmk_ext_power_generic));

static const struct gpio_dt_spec LED_R = GPIO_DT_SPEC_GET(LED_NODE_R, gpios);
static const struct gpio_dt_spec LED_B = GPIO_DT_SPEC_GET(LED_NODE_B, gpios);

void set_led_dt(const struct gpio_dt_spec* spec, enum LED_SWITCH sw)
{
    if (!device_is_ready(spec->port)) {
        LOG_ERR("LED %s is not ready.", spec->port->name);
        return;
    }

    switch (sw) {
    case CLOSE:
        if (spec->dt_flags == GPIO_ACTIVE_HIGH) {
            gpio_pin_configure_dt(spec, GPIO_DISCONNECTED);
        } else if (spec->dt_flags == GPIO_ACTIVE_LOW) {
            gpio_pin_configure_dt(spec, GPIO_DISCONNECTED);
            if (ext_power != NULL) {
                int rc = ext_power_disable(ext_power);
                if (rc != 0) {
                    LOG_ERR("Unable to disable EXT_POWER: %d", rc);
                }
            }
        } else {
            LOG_ERR("LED %s, flag %d, doesn't have property ACTIVE.", spec->port->name, spec->dt_flags);
        }
        break;

    case OPEN:
        if (spec->dt_flags == GPIO_ACTIVE_HIGH) {
            gpio_pin_configure_dt(spec, GPIO_OUTPUT_HIGH);
        } else if (spec->dt_flags == GPIO_ACTIVE_LOW) {
            gpio_pin_configure_dt(spec, GPIO_OUTPUT_LOW);
            if (ext_power != NULL) {
                int rc = ext_power_enable(ext_power);
                if (rc != 0) {
                    LOG_ERR("Unable to enable EXT_POWER: %d", rc);
                }
            }
        } else {
            LOG_ERR("LED %s, flag %d, doesn't have property ACTIVE.", spec->port->name, spec->dt_flags);
        }
        break;

    default:
        LOG_ERR("set_led_dt(): sw %d is illegal.", sw);
        return;
    }
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