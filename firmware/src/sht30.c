#include "gpio_config.h"
#include "sht30.h"
#include "msmart.h"
#include <stdio.h>
#include <unistd.h>

float g_temperature = 0.0f;
float g_humidity = 0.0f;
static unsigned short g_sht30_addr = (SHT30_ADDR << 1) | 0x00;
static int g_sht30ErrCount = 0;

void I2C_Init(void)
{
    IoSetFunc(WIFI_IOT_IO_NAME_GPIO_0, WIFI_IOT_IO_FUNC_GPIO_0_I2C1_SDA);
    IoSetFunc(WIFI_IOT_IO_NAME_GPIO_1, WIFI_IOT_IO_FUNC_GPIO_1_I2C1_SCL);
    I2cInit(SHT30_I2C_IDX, 400000);
    I2cSetBaudrate(SHT30_I2C_IDX, 400000);
    printf("[I2C] Initialized\n");
}

void SHT30_Init(void)
{
    uint8_t cmd[2] = {0x30, 0xA2};
    WifiIotI2cData data = {0};
    data.sendBuf = cmd;
    data.sendLen = 2;

    if (I2cWrite(SHT30_I2C_IDX, (SHT30_ADDR << 1) | 0x00, &data) == WIFI_IOT_SUCCESS) {
        g_sht30_addr = (SHT30_ADDR << 1) | 0x00;
    } else if (I2cWrite(SHT30_I2C_IDX, ((SHT30_ADDR + 1) << 1) | 0x00, &data) == WIFI_IOT_SUCCESS) {
        g_sht30_addr = ((SHT30_ADDR + 1) << 1) | 0x00;
    }
    printf("[SHT30] addr=0x%02X\n", g_sht30_addr);
}

void SHT30_Read(void)
{
    uint8_t cmd[2] = {0x2C, 0x06};
    uint8_t recv_buf[6] = {0};
    WifiIotI2cData data = {0};
    int ok = 0;
    int retry;

    for (retry = 0; retry < 3; retry++) {
        data.sendBuf = cmd;
        data.sendLen = 2;
        if (I2cWrite(SHT30_I2C_IDX, g_sht30_addr, &data) != WIFI_IOT_SUCCESS) {
            continue;
        }
        usleep(15000);

        data.sendBuf = NULL;
        data.sendLen = 0;
        data.receiveBuf = recv_buf;
        data.receiveLen = 6;
        if (I2cRead(SHT30_I2C_IDX, g_sht30_addr | 0x01, &data) != WIFI_IOT_SUCCESS) {
            continue;
        }
        ok = 1;
        break;
    }

    if (!ok) {
        g_sht30ErrCount++;
        if (g_sht30ErrCount % 10 == 1) {
            printf("[SHT30] Read failed (sensor may be disconnected), keep last value T=%.1fC H=%.1f%%\n",
                   g_temperature, g_humidity);
        }
        return;
    }

    g_sht30ErrCount = 0;
    uint16_t temp_raw = (uint16_t)((recv_buf[0] << 8) | recv_buf[1]);
    uint16_t hum_raw  = (uint16_t)((recv_buf[3] << 8) | recv_buf[4]);
    g_temperature = 175.0f * (float)temp_raw / 65535.0f - 45.0f;
    g_humidity    = 100.0f * (float)hum_raw  / 65535.0f;

    if (g_acOn) {
        printf("[SHT30] T=%.1fC H=%.1f%%  [AC ON: %s %.1fC]\n",
               g_temperature, g_humidity, g_acMode, (float)g_acTargetTemp);
    } else {
        printf("[SHT30] T=%.1fC H=%.1f%%\n", g_temperature, g_humidity);
    }
}
