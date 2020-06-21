/*
 * Copyright (c) 2020 Endian Technologies AB
 * Copyright (c) 2020 max00
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include <zephyr.h>
#include <zephyr/types.h>
#include <drivers/gpio.h>
#include <drivers/sensor.h>
#include <stdbool.h>
#include "backlight.h"
#include "battery.h"
#include "bt.h"
#include "clock.h"
#include "display.h"
#include "event_handler.h"
#include "gfx.h"
#include "log.h"

/* ********** defines ********** */
#define BAT_CHA 12
//#define BTN_PORT DT_NODELABEL(button0)
//#define BTN_IN  DT_NODELABEL(button0)
#define BTN_IN_PIN 14
#define BTN_OUT_PIN 15
#define TOUCH_PORT  CONFIG_CST816S_NAME
#define DISPLAY_TIMEOUT K_SECONDS(5)
/* ********** ******* ********** */

/* ********** variables ********** */
static struct k_timer display_off_timer;
static struct device *charging_dev;
static struct gpio_callback charging_cb;
static struct device *button_dev;
static struct gpio_callback button_cb;
static struct device *touch_dev;
static struct sensor_trigger tap = {
	.type = SENSOR_TRIG_DATA_READY,
	.chan = SENSOR_CHAN_ACCEL_XYZ,
};
/* ********** ********* ********** */

/* ********** init function ********** */
void event_handler_init()
{
	/* Initialize GPIOs */
    charging_dev = device_get_binding("GPIO_0");
    gpio_pin_configure(charging_dev, BAT_CHA, GPIO_INPUT | GPIO_INT_EDGE_BOTH);
    gpio_init_callback(&charging_cb, battery_charging_isr, BIT(BAT_CHA));

	button_dev = device_get_binding("GPIO_0");
	gpio_pin_configure(button_dev, BTN_IN_PIN, GPIO_INPUT | GPIO_INT_EDGE_FALLING | GPIO_PULL_UP);
	gpio_init_callback(&button_cb, button_pressed_isr, BIT(BTN_IN_PIN));

	touch_dev = device_get_binding(TOUCH_PORT);

	/* Enable GPIOs */
	gpio_add_callback(charging_dev, &charging_cb);
	gpio_add_callback(button_dev, &button_cb);
	sensor_trigger_set(touch_dev, &tap, touch_tap_isr);

	/* Set button out pin to high to enable the button */
	uint32_t button_out = 1U;
        gpio_pin_configure(button_dev, BTN_OUT_PIN, GPIO_OUTPUT);
        gpio_pin_set_raw(button_dev, BTN_OUT_PIN, button_out);

	/* Initialize timers */
	k_timer_init(&display_off_timer, display_off_isr, NULL);

	/* Start timers */
	k_timer_start(&display_off_timer, DISPLAY_TIMEOUT, K_NO_WAIT);

	/* Special cases */
        /* Get battery charging status */
	k_sleep(K_MSEC(10));
        uint32_t res =  gpio_pin_get(charging_dev, BAT_CHA);
        battery_update_charging_status(res != 1U);

        /* Show time, date and battery status */
	clock_show_time();
	battery_show_status();

	LOG_DBG("Event handler init: Done");
}
/* ********** ************ ********** */

/* ********** interrupt handlers ********** */
void display_off_isr(struct k_timer *light_off)
{
	backlight_enable(false);
	display_sleep();
}

void battery_charging_isr(struct device *gpiobat, struct gpio_callback *cb, uint32_t pins)
{
	uint32_t res = gpio_pin_get(charging_dev, BAT_CHA);
	battery_update_charging_status(res != 1U);
}

void button_pressed_isr(struct device *gpiobtn, struct gpio_callback *cb, uint32_t pins)
{
	backlight_enable(true);
	k_timer_start(&display_off_timer, DISPLAY_TIMEOUT, K_NO_WAIT);
	display_wake_up();
	clock_show_time();
	battery_show_status();
	gfx_update();
}

void touch_tap_isr(struct device *touch_dev, struct sensor_trigger *tap)
{
	backlight_enable(true);
	k_timer_start(&display_off_timer, DISPLAY_TIMEOUT, K_NO_WAIT);
	display_wake_up();
	clock_increment_local_time();
	clock_show_time();
	battery_show_status();
	gfx_update();
}

/* ********** ************** ********** */
