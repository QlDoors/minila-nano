/*
 * Copyright (c) 2021 Megamind (megamind4089)
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#define CONFIG_MINILA_BATTERY_LED 1

#if (CONFIG_MINILA_BATTERY_LED)

#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zephyr/drivers/gpio.h>
#include <zmk/usb.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>

#include "led.h"

static bool is_blink = 0;
static bool is_on = 0;

void led_blink_timer_fn() {
    LOG_WRN("**** led_blink_handler: start: is_blink = %d, is_on = %d", is_blink, is_on);
    if (is_on == 0) {
        set_led(RED, GPIO_OUTPUT_HIGH);
        is_on = 1;
    } else {
        set_led(RED, GPIO_DISCONNECTED);
        is_on = 0;
    }
}

K_TIMER_DEFINE(led_blink_timer, led_blink_timer_fn, NULL);

void led_start_blink_timer() {
    LOG_WRN("**** led_start_blink.");
    k_timer_start(&led_blink_timer, K_NO_WAIT, K_SECONDS(1));
    is_blink = 1;
}

void led_stop_blink_timer() {
    LOG_DBG("**** led_stop_blink_timer()");
    k_timer_stop(&led_blink_timer);
    is_blink = 0;
    is_on = 0;
}

static int battery_listener(const zmk_event_t *eh) {
    const struct zmk_battery_state_changed *ev = as_zmk_battery_state_changed(eh);

    uint8_t level = (ev != NULL) ? ev->state_of_charge : zmk_battery_state_of_charge();
    bool usb_present = zmk_usb_is_powered();

    LOG_DBG("**** minila battery_listener: battery changed, level = %d, usb = %d", level, usb_present);
    if (level < 100) {
        if (usb_present) {
            led_stop_blink_timer();
            set_led(RED, GPIO_OUTPUT_HIGH);
        // } else if (level <= 96) {   // for test
        } else if (level <= 20) {
            if (is_blink == 0) {
                led_start_blink_timer();
            }
        }
    } else {
        led_stop_blink_timer();
        set_led(RED, GPIO_DISCONNECTED);
    }

    return ZMK_EV_EVENT_BUBBLE;
}


ZMK_LISTENER(minila_battery, battery_listener);
ZMK_SUBSCRIPTION(minila_battery, zmk_battery_state_changed);
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
ZMK_SUBSCRIPTION(minila_battery, zmk_usb_conn_state_changed);
#endif /* IS_ENABLED(CONFIG_USB_DEVICE_STACK) */

#endif // #if (MINILA_BATTERY_LED)