#include "gpio_config.h"
#include "door.h"
#include "auto_lock.h"
#include "udp_server.h"
#include "cmsis_os2.h"
#include <stdio.h>

int g_door_open = 0;

void Door_Init(void)
{
    IoSetFunc(DOOR_GPIO, WIFI_IOT_IO_FUNC_GPIO_9_GPIO);
    GpioSetDir(DOOR_GPIO, WIFI_IOT_GPIO_DIR_IN);
    IoSetPull(DOOR_GPIO, WIFI_IOT_IO_PULL_UP);
    printf("[DoorSensor] Initialized\n");
}

void Door_Read(void)
{
    WifiIotGpioValue val = WIFI_IOT_GPIO_VALUE0;
    GpioGetInputVal(DOOR_GPIO, &val);
    int door_open = (val == WIFI_IOT_GPIO_VALUE0) ? 1 : 0;

    if (door_open != g_door_open) {
        g_door_open = door_open;
        if (g_door_open) {
            printf("[Door] Door OPENED\n");
            UDP_SendMessage("{\"event\":\"door_open\"}");
            if (g_auto_unlock_active) {
                g_auto_unlock_door_opened = 1;
            }
        } else {
            printf("[Door] Door CLOSED\n");
            if (g_auto_unlock_active && g_auto_unlock_door_opened) {
                extern uint32_t g_auto_unlock_start;
                g_auto_unlock_start = osKernelGetTickCount();
                printf("[Auto] Door closed, 5s timer started\n");
            }
        }
    }
}
