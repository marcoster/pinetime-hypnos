/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <stdbool.h>
#include <drivers/gpio.h>
#include <display.h>
#include "backlight.h"
#include "log.h"

/* ********** ********** DEFINES ********** ********** ********** */
//#define BACKLIGHT_PORT  DT_ALIAS_LED1_GPIOS_CONTROLLER
//#define BACKLIGHT       DT_ALIAS_LED1_GPIOS_PIN
#define BACKLIGHT_PIN   22
/* ********** **********  ********** ********** ********** */

/* ********** ********** VARIABLES AND STRUCTS ********** ********** */
static struct device* backlight_dev;
static bool backlight_enabled = false;
/* ********** ********** ********** ********** ********** ********** */

/* ********** ********** FUNCTIONS ********** ********** */
void backlight_init()
{
	backlight_dev = device_get_binding("GPIO_0");
	gpio_pin_configure(backlight_dev, BACKLIGHT_PIN, GPIO_OUTPUT);
	backlight_enable(true);
	LOG_DBG("Backlight init: Done");
}

void backlight_enable(bool enable) {
	gpio_pin_set_raw(backlight_dev, BACKLIGHT_PIN, enable ? 0 : 1);
	backlight_enabled = enable;
}

bool backlight_is_enabled()
{
	return backlight_enabled;
}
/* ********** ********** ********** ********** ********** */
