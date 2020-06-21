/*
 * PineTime Hypnos: Smartwatch firmware for the PineTime dev kit
 * Copyright (c) 2020 Endian Technologies AB
 *
 * Modifications of endian-albin's pinetime-hypnos:
 * Copyright (c) 2020 max00
 *
 * This is free software with ABSOLUTELY NO WARRANTY.
 *
 * SPDX-License-Identifier: MPL-2.0
 */

#include <zephyr.h>
#include <settings/settings.h>
#include "backlight.h"
#include "battery.h"
#include "bt.h"
#include "clock.h"
#include "cts_sync.h"
#include "display.h"
#include "event_handler.h"
#include "gfx.h"
#include "log.h"

void main(void)
{
	LOG_INF("Welcome to PineTime Hypnos!");
    LOG_INF("This is free software with ABSOLUTELY NO WARRANTY.");

    if(IS_ENABLED(CONFIG_SETTINGS)) {
        settings_load();
    }

    ble_init();
    cts_sync_init();
	gfx_init();
	clock_init();
	battery_init();
	display_init();
	event_handler_init();
	gfx_update();
	backlight_init();
}

