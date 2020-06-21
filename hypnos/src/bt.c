/*
 * Copyright (c) 2015-2016 Intel Corporation
 * Copyright (c) 2020 Endian Technologies AB
 * Copyright (c) 2020 max00
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <sys/byteorder.h>
#include <zephyr.h>
#include <zephyr/types.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include "cts_sync.h"
#include "log.h"
#include "gfx.h"

//---- DEFINITIONS ----//
#define BT_TIMEOUT K_SECONDS(20)


//---- PROTOTYPES ----//
static void ready(int err);
static void connected(struct bt_conn *conn, uint8_t err);
static void disconnected(struct bt_conn *conn, uint8_t reason);
static bool le_param_req(struct bt_conn *conn, struct bt_le_conn_param *param);
static void le_param_updated(struct bt_conn *conn, uint16_t interval, uint16_t latency, uint16_t timeout);


//---- VARIABLES ----//
static const struct bt_data ad[] = {
        BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
        BT_DATA_BYTES(BT_DATA_UUID16_ALL,
              0x22, 0x18, 0x0d, 0x18, 0x05, 0x18),
//        BT_DATA_BYTES(BT_DATA_UUID128_ALL,
//              0xf0, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12,
//              0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12),
};
static struct bt_conn_cb m_conn_callbacks = {
    .connected = connected,
    .disconnected = disconnected,
    .le_param_req = le_param_req,
    .le_param_updated = le_param_updated
};


//---- FUNCTIONS ----//
void ble_init(void)
{
    int err = bt_enable(ready);
    if(err) {
        LOG_ERR("BLE init failed (err %d)", err);
    }
}

bool ble_is_connected(void)
{
    return false;
}


static void ready(int err)
{
    if(err) {
        LOG_ERR("BLE init failed (err %d)", err);
        return;
    }

    bt_conn_cb_register(&m_conn_callbacks);

    struct bt_le_adv_param param = BT_LE_ADV_PARAM_INIT(
        BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_USE_NAME,
        BT_GAP_ADV_SLOW_INT_MIN,
        BT_GAP_ADV_SLOW_INT_MAX,
        NULL
            );
    err = bt_le_adv_start(&param, ad, ARRAY_SIZE(ad), NULL, 0);
    if(err) {
        LOG_ERR("BLE adv start failed (err %d)", err);
    }
}

static void connected(struct bt_conn *conn, uint8_t err)
{
    LOG_INF("connected %u", err);
    if(err) {
        return;
    }
    cts_sync_enable(true);
    gfx_bt_set_label(1);
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    LOG_INF("disconnected");
    cts_sync_enable(false);
    gfx_bt_set_label(0);
}

static bool le_param_req(struct bt_conn *conn, struct bt_le_conn_param *param)
{
    return true;
}

static void le_param_updated(struct bt_conn *conn, uint16_t interval, uint16_t latency, uint16_t timeout)
{

}
