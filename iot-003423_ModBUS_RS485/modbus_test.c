/*
 * Copy from libmodbus/tests/version.c
 * 
 * Copyright © 2008-2014 Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


/*
 *  電腦向表頭發送   ： 設備站號 命令 開始位址 需要讀取數目  CRC  校驗
 *  表頭返回        ： 設備站號 命令 資料大小 有效資料     CRC  校驗
 *
 *  
 *  Read  Register：  Reg 0-1
 *  send command                  0x01 0x03 0x00 0x00 0x00 0x02 0xC4 0x0B
 *  get  positive Tempersature    0x01 0x03 0x04 0x00 0xFA 0x03 0xC8 0xDB 0x64
 *  get  negitive Tempersature    0x01 0x03 0x04 0xFF 0x69 0x03 0xB8 0x1A 0xB9
 * 
 */
 
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <modbus.h>

#define SERVER_ID 0x01
#define REGISTER_CNT 2

int main(void)
{
    uint32_t new_response_to_sec;
    uint32_t new_response_to_usec;
    modbus_t *ctx = NULL;
    uint16_t *tab_rp_registers;
    const uint16_t UT_REGISTERS_ADDRESS = 0x00;
    const uint16_t UT_INPUT_REGISTERS_ADDRESS = 0x00;
    int i, rc;

    ctx = modbus_new_rtu("/dev/ttyUSB0", 9600, 'N', 8, 1);
    if (ctx == NULL) {
        fprintf(stderr, "Unable to allocate libmodbus context\n");
        return -1;
    }

    modbus_set_slave(ctx, SERVER_ID);

    new_response_to_sec = 5;
    new_response_to_usec = 500;
    modbus_set_response_timeout(ctx, new_response_to_sec, new_response_to_usec);

    modbus_set_debug(ctx, TRUE);
    modbus_set_error_recovery(ctx,
                              MODBUS_ERROR_RECOVERY_LINK |
                              MODBUS_ERROR_RECOVERY_PROTOCOL);

    if (modbus_connect(ctx) == -1) {
        fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }

    tab_rp_registers = (uint16_t *)malloc(REGISTER_CNT * sizeof(uint16_t));

    rc = modbus_read_registers(ctx, UT_REGISTERS_ADDRESS,
        REGISTER_CNT, tab_rp_registers);
    if (rc == -1) {
        fprintf(stderr, "%s\n", modbus_strerror(errno));
        return -1;
    }

    for (i = 0; i < REGISTER_CNT; i++)
        printf("tab_rp_registers[%d] = %d\r\n", i, tab_rp_registers[i]);

    free(tab_rp_registers);
    return 0;
}
