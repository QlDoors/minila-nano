/*
 * Copyright (c) 2021 Megamind (megamind4089)
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#define CONFIG_MINILA_BLE_LED 1
#if CONFIG_MINILA_BLE_LED

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(minila_ble_led, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/events/ble_active_profile_changed.h>

#include "led.h"

void ble_led_timer_fn()
{
    set_led(BLUE, CLOSE);
}

K_TIMER_DEFINE(ble_led_timer, ble_led_timer_fn, NULL);

int ble_led_listener(const zmk_event_t* eh)
{
    const struct zmk_ble_active_profile_changed* profile_ev = as_zmk_ble_active_profile_changed(eh);

    if (profile_ev == NULL) {
        return ZMK_EV_EVENT_BUBBLE;
    }
    LOG_DBG("ble_led_listener: Active profile index: %d", profile_ev->index);

    set_led(BLUE, OPEN);
    k_timer_start(&ble_led_timer, K_SECONDS(3), K_NO_WAIT);
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(led_output_status, ble_led_listener)
#if defined(CONFIG_ZMK_BLE)
ZMK_SUBSCRIPTION(led_output_status, zmk_ble_active_profile_changed);
#endif
#endif