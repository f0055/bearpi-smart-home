#include "gpio_config.h"
#include "button.h"
#include "led.h"
#include "servo.h"
#include "udp_server.h"
#include <stdio.h>

static int g_buttonF1Last = 0;
static int g_buttonF1Debounce = 0;
static int g_buttonF2Last = 0;
static int g_buttonF2Debounce = 0;

void Button_Init(void)
{
    IoSetFunc(BUTTON_F1_GPIO, WIFI_IOT_IO_FUNC_GPIO_11_GPIO);
    GpioSetDir(BUTTON_F1_GPIO, WIFI_IOT_GPIO_DIR_IN);
    IoSetPull(BUTTON_F1_GPIO, WIFI_IOT_IO_PULL_UP);

    IoSetFunc(BUTTON_F2_GPIO, WIFI_IOT_IO_FUNC_GPIO_12_GPIO);
    GpioSetDir(BUTTON_F2_GPIO, WIFI_IOT_GPIO_DIR_IN);
    IoSetPull(BUTTON_F2_GPIO, WIFI_IOT_IO_PULL_UP);

    printf("[Button] Initialized (F1=GPIO_11, F2=GPIO_12)\n");
}

void Button_Read(void)
{
    WifiIotGpioValue val = WIFI_IOT_GPIO_VALUE0;

    GpioGetInputVal(BUTTON_F1_GPIO, &val);
    int f1_now = (val == WIFI_IOT_GPIO_VALUE0) ? 1 : 0;

    if (f1_now != g_buttonF1Last) {
        g_buttonF1Debounce = 5;
    }
    if (g_buttonF1Debounce > 0) {
        g_buttonF1Debounce--;
        if (g_buttonF1Debounce == 0 && f1_now == 1) {
            if (g_ledOn) {
                LED_Off();
                printf("[F1] LED OFF (local)\n");
            } else {
                LED_On();
                printf("[F1] LED ON (local)\n");
            }
            PublishStatus();
        }
        g_buttonF1Last = f1_now;
    }

    GpioGetInputVal(BUTTON_F2_GPIO, &val);
    int f2_now = (val == WIFI_IOT_GPIO_VALUE0) ? 1 : 0;

    if (f2_now != g_buttonF2Last) {
        g_buttonF2Debounce = 5;
    }
    if (g_buttonF2Debounce > 0) {
        g_buttonF2Debounce--;
        if (g_buttonF2Debounce == 0 && f2_now == 1) {
            if (g_locked) {
                Door_Unlock();
                printf("[F2] Door UNLOCK (local)\n");
            } else {
                Door_Lock();
                printf("[F2] Door LOCK (local)\n");
            }
            PublishStatus();
        }
        g_buttonF2Last = f2_now;
    }
}
