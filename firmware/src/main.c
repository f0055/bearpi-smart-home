#include "gpio_config.h"
#include "led.h"
#include "servo.h"
#include "pir.h"
#include "door.h"
#include "button.h"
#include "sht30.h"
#include "wifi.h"
#include "udp_server.h"
#include "msmart.h"
#include "schedule.h"
#include "state.h"
#include "auto_lock.h"
#include "ohos_init.h"
#include "cmsis_os2.h"
#include <stdio.h>
#include <string.h>

static void SmartLight_Task(void)
{
    int sht30Counter = 0;

    printf("\n========================================\n");
    printf("  Smart Light Controller v2.0 (UDP)\n");
    printf("  BearPi-HM_Nano (Hi3861)\n");
    printf("  Build: %s %s\n", __DATE__, __TIME__);
    printf("========================================\n\n");

    memset(g_timers, 0, sizeof(g_timers));

    LED_Init();
    Servo_Init();
    PIR_Init();
    Door_Init();
    Button_Init();
    I2C_Init();
    SHT30_Init();

    State_Load();

    if (g_ledOn && g_brightness > 0) {
        LED_SetBrightness(g_brightness);
    } else {
        g_ledOn = 0;
        g_brightness = 0;
    }
    if (WiFiConnect(WIFI_SSID, WIFI_PASSWORD) != 0) {
        printf("[ERROR] WiFi connection failed\n");
        while (1) osDelay(1000);
    }

    printf("\n[System] Smart Light is ready!\n");
    printf("[System] UDP port: %d\n\n", UDP_PORT);

    osThreadAttr_t udpAttr = {0};
    udpAttr.name = "UDP_Recv";
    udpAttr.stack_size = 8192;
    udpAttr.priority = 24;
    osThreadNew((osThreadFunc_t)UDP_Recv_Task, NULL, &udpAttr);

    while (1) {
        osDelay(100);
        g_uptime++;

        WiFi_AutoReconnect();
        Button_Read();
        PIR_Read();
        Door_Read();

        if (++sht30Counter >= 20) {
            sht30Counter = 0;
            SHT30_Read();
        }

        AutoLock_Update();

        g_pir_last = g_pir;

        Schedule_CheckTimers();

        if (g_stateDirty) {
            State_Save();
            g_stateDirty = 0;
        }
    }
}

static void SmartLight_Init(void)
{
    osThreadAttr_t attr = {0};
    attr.name = "SmartLight";
    attr.stack_size = 16384;
    attr.priority = 24;

    if (osThreadNew((osThreadFunc_t)SmartLight_Task, NULL, &attr) == NULL) {
        printf("[ERROR] Failed to create SmartLight task\n");
    }

    osThreadAttr_t bcastAttr = {0};
    bcastAttr.name = "Broadcast";
    bcastAttr.stack_size = 4096;
    bcastAttr.priority = 20;

    if (osThreadNew((osThreadFunc_t)UDP_Broadcast_Task, NULL, &bcastAttr) == NULL) {
        printf("[ERROR] Failed to create Broadcast task\n");
    }
}

APP_FEATURE_INIT(SmartLight_Init);
