#include "gpio_config.h"
#include "pir.h"
#include <stdio.h>

int g_pir = 0;
int g_pir_last = 0;

void PIR_Init(void)
{
    IoSetFunc(PIR_GPIO, WIFI_IOT_IO_FUNC_GPIO_6_GPIO);
    GpioSetDir(PIR_GPIO, WIFI_IOT_GPIO_DIR_IN);
    IoSetPull(PIR_GPIO, WIFI_IOT_IO_PULL_UP);
    printf("[PIR] Initialized\n");
}

void PIR_Read(void)
{
    WifiIotGpioValue val = WIFI_IOT_GPIO_VALUE0;
    GpioGetInputVal(PIR_GPIO, &val);
    int pir_new = (val == WIFI_IOT_GPIO_VALUE0) ? 1 : 0;
    if (pir_new != g_pir) {
        g_pir = pir_new;
        printf("[PIR] %s\n", g_pir ? "DETECTED" : "CLEARED");
    }
}
