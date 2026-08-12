#include "gpio_config.h"
#include "led.h"
#include "udp_server.h"
#include <stdio.h>

int g_ledOn = 0;
int g_brightness = 0;

void LED_On(void)
{
    PwmStart(LED_PWM_PORT, PWM_DUTY_MAX, PWM_DUTY_MAX);
    g_ledOn = 1;
    g_brightness = 100;
    printf("[LED] Turn ON, brightness=%d%%\n", g_brightness);
    PublishStatus();
}

void LED_Off(void)
{
    PwmStop(LED_PWM_PORT);
    g_ledOn = 0;
    g_brightness = 0;
    printf("[LED] Turn OFF\n");
    PublishStatus();
}

void LED_SetBrightness(int brightness)
{
    if (brightness < 0) brightness = 0;
    if (brightness > 100) brightness = 100;

    g_brightness = brightness;

    if (brightness == 0) {
        LED_Off();
    } else {
        unsigned int duty = (unsigned int)(brightness * PWM_DUTY_MAX / 100);
        PwmStart(LED_PWM_PORT, duty, PWM_DUTY_MAX);
        g_ledOn = 1;
        printf("[LED] Brightness: %d%%\n", brightness);
    }
}

void LED_Init(void)
{
    GpioInit();
    IoSetFunc(LED_GPIO, WIFI_IOT_IO_FUNC_GPIO_2_PWM2_OUT);
    GpioSetDir(LED_GPIO, WIFI_IOT_GPIO_DIR_OUT);
    PwmInit(LED_PWM_PORT);
    PwmStop(LED_PWM_PORT);
    g_ledOn = 0;
    g_brightness = 0;
    printf("[LED] Initialized\n");
}
