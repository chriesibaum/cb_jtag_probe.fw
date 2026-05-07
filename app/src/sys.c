// SPDX-License-Identifier: GPL-3.0
// Copyright (c) 2026 Chriesibaum GmbH

// ---- system includes -------------------------------------------------------
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

// ---- logging includes/defines ----------------------------------------------
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(SYS, CONFIG_LOG_DEFAULT_LEVEL);

// Application includes
#include "sys.h"


#define LED_SYS_NODE DT_ALIAS(sys_led)
#define LED_TOGGLE_PERIOD_MS 1000   // Toggle period in milliseconds
#define LED_BLINK_DURATION_MS  50   // LED on duration in milliseconds


static const struct gpio_dt_spec led_sys = GPIO_DT_SPEC_GET(LED_SYS_NODE, gpios);

// sys led timer
static void sys_led_timer_handler(struct k_timer *timer);
K_TIMER_DEFINE(sys_led_timer, sys_led_timer_handler, NULL);

static void sys_led_timer_off_handler(struct k_timer *timer);
K_TIMER_DEFINE(sys_led_timer_off, sys_led_timer_off_handler, NULL);

static void sys_led_timer_handler(struct k_timer *timer)
{
    gpio_pin_set_dt(&led_sys, 1);
    k_timer_start(&sys_led_timer_off, K_MSEC(LED_BLINK_DURATION_MS), K_NO_WAIT);
}

static void sys_led_timer_off_handler(struct k_timer *timer)
{
    gpio_pin_set_dt(&led_sys, 0);
}

int32_t sys_init(void)
{
    if (!gpio_is_ready_dt(&led_sys)) {
        return -1;
    }

    int32_t ret = gpio_pin_configure_dt(&led_sys, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        return ret;
    }

    /* Start the timer to toggle LED every LED_TOGGLE_PERIOD_MS */
    k_timer_start(&sys_led_timer, K_MSEC(LED_TOGGLE_PERIOD_MS), K_MSEC(LED_TOGGLE_PERIOD_MS));

    return 0;
}