/*
 * Copyright (c) 2022 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#if (CONFIG_MINILA_INDICATORS_LED)

#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/events/hid_indicators_changed.h>
#include <zmk/hid_indicators.h>

#include "led.h"

static int pre_indicator = 0;

static int indicators_listener(const zmk_event_t* eh)
{
    struct zmk_hid_indicators_changed* ev = as_zmk_hid_indicators_changed(eh);
    if (ev == NULL) {
        LOG_ERR("****indicators_listener: ev == NULL, ERR!!!");
        return -ENOTSUP;
    }

    if (ev->indicators != pre_indicator) {
        LOG_DBG("****indicators_listener: indicators changed, pre = %d, cur = %d", pre_indicator, ev->indicators);
        if (ev->indicators == HID_USAGE_LED_CAPS_LOCK) {
            set_led(BLUE, OPEN);
        } else {
            set_led(BLUE, CLOSE);
        }
        pre_indicator = ev->indicators;
    }
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(minila_indicators, indicators_listener);
ZMK_SUBSCRIPTION(minila_indicators, zmk_hid_indicators_changed);

#endif // #if (CONFIG_MINILA_INDICATORS_LED)