#include "gpio_config.h"
#include "auto_lock.h"
#include "servo.h"
#include "pir.h"
#include "udp_server.h"
#include "cmsis_os2.h"
#include <stdio.h>

int g_auto_unlock_active = 0;
int g_auto_unlock_door_opened = 0;
int g_autoUnlockEnabled = 0;

static uint32_t g_auto_unlock_start = 0;
static uint32_t g_auto_unlock_detect = 0;

void AutoLock_SetEnabled(int enabled)
{
    g_autoUnlockEnabled = enabled;
    printf("[Auto] Auto-unlock %s\n", g_autoUnlockEnabled ? "ENABLED" : "DISABLED");
}

void AutoLock_Update(void)
{
    /* 5秒无条件上锁 */
    if (g_auto_unlock_active) {
        uint32_t elapsed = osKernelGetTickCount() - g_auto_unlock_start;
        uint32_t timeoutTicks = (uint32_t)AUTO_UNLOCK_SEC * osKernelGetTickFreq();

        if (elapsed >= timeoutTicks) {
            Door_Lock();
            g_auto_unlock_active = 0;
            g_auto_unlock_door_opened = 0;
            g_auto_unlock_start = 0;
            printf("[Auto] 5s auto locked\n");
            PublishStatus();
        }
    }

    /* 自动门禁控制 */
    if (g_autoUnlockEnabled) {
        if (g_pir && !g_pir_last) {
            printf("[Auto] Person detected, auto unlocking\n");
            Door_Unlock();
            g_auto_unlock_active = 1;
            g_auto_unlock_door_opened = 0;
            g_auto_unlock_start = osKernelGetTickCount();
            g_auto_unlock_detect = osKernelGetTickCount();
        }
    }

    /* PIR事件推送 */
    if (g_pir && !g_pir_last) {
        char pirEvent[] = "{\"event\":\"pir_detected\"}";
        sendto(g_udpSocket, pirEvent, strlen(pirEvent), 0,
               (struct sockaddr *)&g_appAddr, sizeof(g_appAddr));
        printf("[PIR] Person detected, sending event\n");
        PublishStatus();
    }
    if (!g_pir && g_pir_last) {
        char pirLeftEvent[] = "{\"event\":\"pir_left\"}";
        sendto(g_udpSocket, pirLeftEvent, strlen(pirLeftEvent), 0,
               (struct sockaddr *)&g_appAddr, sizeof(g_appAddr));
        printf("[PIR] Person left, sending event\n");
        PublishStatus();
    }
}
