/*
 * Copyright (c) 2021 Megamind (megamind4089)
 *
 * SPDX-License-Identifier: Apache-2.0
 */
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


static int battery_listener(const zmk_event_t *eh) {
    const struct zmk_battery_state_changed *ev = as_zmk_battery_state_changed(eh);

    uint8_t level = (ev != NULL) ? ev->state_of_charge : zmk_battery_state_of_charge();
    bool usb_present = zmk_usb_is_powered();

    LOG_DBG("****battery_listener: battery changed, level = %d, usb = %d", level, usb_present);
    if (level < 100) {
        if (usb_present) {
            set_led(RED, GPIO_OUTPUT_HIGH);
        // } else if (level <= 96) {   // for test
        } else if (level <= 20) {
            // TODO: blink
            set_led(RED, GPIO_OUTPUT_HIGH);
        }
    } else {
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