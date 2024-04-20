/*
 * Copyright (c) 2021 Megamind (megamind4089)
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#if (CONFIG_MINILA_BATTERY_LED)

#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(minila_battery, CONFIG_ZMK_LOG_LEVEL);

#include <zephyr/drivers/gpio.h>
#include <zmk/battery.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/usb.h>

#include "led.h"

static uint8_t battery_level = 0;
static const uint8_t BLINK_LEVEL = 20;
// static const uint8_t BLINK_LEVEL = 96; // for test

static volatile bool is_blink = false;
static volatile bool is_on = false;

static const double charge_current = 180.0; // unit mA
static const uint32_t BATTERY_VOL = CONFIG_MINILA_BATTERY_VOL; // unit mAH
static uint32_t charge_complete_time = 0;
static volatile bool is_charging = false;

void led_stop_blink_timer();

void led_blink_timer_fn()
{
    LOG_DBG("start: is_blink = %d, is_on = %d", is_blink, is_on);
    if (is_charging || battery_level > BLINK_LEVEL) {
        led_stop_blink_timer();
        return;
    }

    if (is_on == 0) {
        set_led(RED, OPEN);
        is_on = 1;
    } else {
        set_led(RED, CLOSE);
        is_on = 0;
    }
}

K_TIMER_DEFINE(led_blink_timer, led_blink_timer_fn, NULL);

void led_start_blink_timer()
{
    LOG_DBG("**** led_start_blink.");
    k_timer_start(&led_blink_timer, K_NO_WAIT, K_SECONDS(1));
    is_blink = 1;
}

void led_stop_blink_timer()
{
    LOG_DBG("**** led_stop_blink_timer()");
    k_timer_stop(&led_blink_timer);
    is_blink = 0;
    is_on = 0;
}

void stop_battery_report_timer();

void battery_report_timer_fn()
{
    uint32_t cur_time = k_uptime_get_32();
    LOG_DBG("cur_time = %d, charge_complete_time = %d, is_charging = %d", cur_time, charge_complete_time, is_charging);
    if (charge_complete_time <= cur_time || is_charging == 0) {
        set_led(RED, CLOSE);
        stop_battery_report_timer();
    }
}

K_TIMER_DEFINE(battery_report_timer, battery_report_timer_fn, NULL);

void start_battery_report_timer()
{
    LOG_DBG("start_battery_report_timer()");
    k_timer_start(&battery_report_timer, K_NO_WAIT, K_SECONDS(CONFIG_ZMK_BATTERY_REPORT_INTERVAL));
}

void stop_battery_report_timer()
{
    LOG_DBG("stop_battery_report_timer()");
    k_timer_stop(&battery_report_timer);
}

static int battery_listener(const zmk_event_t* eh)
{
    const struct zmk_battery_state_changed* ev = as_zmk_battery_state_changed(eh);

    battery_level = (ev != NULL && ev->state_of_charge > 0) ? ev->state_of_charge : zmk_battery_state_of_charge();
    bool usb_present = zmk_usb_is_powered();

    LOG_DBG("level = %d, usb = %d, is_charging = %d", battery_level, usb_present, is_charging);
    if (battery_level < 100) {
        if (usb_present) {
            if (is_charging == false) { // First time usb_present is detected.
                is_on = 0;  // To prevent led_blink_timer_fn() to close LED.
                is_charging = true;    // To stop led_blink_timer
                set_led(RED, OPEN);

                uint32_t charge_remaining_vol = (BATTERY_VOL - BATTERY_VOL * battery_level / 100);
                charge_complete_time = k_uptime_get_32() + 1.2 * (charge_remaining_vol / charge_current) * 1000 * 3600; // 1.2 is a compensation factor.
                start_battery_report_timer();
            }
        } else {
            if (battery_level <= BLINK_LEVEL) {
                if (is_blink == false) {
                    led_start_blink_timer();
                }
            }
        }
    }

    if (usb_present == false && is_charging) {
        stop_battery_report_timer();
        set_led(RED, CLOSE);
    }

    is_charging = usb_present;
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(minila_battery, battery_listener);
ZMK_SUBSCRIPTION(minila_battery, zmk_battery_state_changed);
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
ZMK_SUBSCRIPTION(minila_battery, zmk_usb_conn_state_changed);
#endif /* IS_ENABLED(CONFIG_USB_DEVICE_STACK) */

#endif // #if (MINILA_BATTERY_LED)