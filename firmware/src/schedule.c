#include "gpio_config.h"
#include "schedule.h"
#include "led.h"
#include "udp_server.h"
#include <stdio.h>
#include <string.h>

Timer g_timers[MAX_TIMERS];
TimeSchedule g_schedules[MAX_SCHEDULES];
int g_scheduleCount = 0;
int g_currentHour = 0;
int g_currentMinute = 0;

void Schedule_CheckTimers(void)
{
    static int lastMinute = -1;

    for (int i = 0; i < MAX_TIMERS; i++) {
        if (g_timers[i].enabled && g_timers[i].delay_sec > 0 &&
            g_uptime >= g_timers[i].trigger_uptime) {
            printf("[Timer] Timer %d triggered (delay %ds)\n", i, g_timers[i].delay_sec);
            g_timers[i].enabled = 0;
            if (g_timers[i].action == 1) {
                LED_On();
            } else if (g_timers[i].action == 2) {
                if (g_ledOn) LED_Off(); else LED_On();
            } else {
                LED_Off();
            }
            PublishStatus();
        }
    }

    int currentHour = (g_uptime / 3600) % 24;
    int currentMinute = (g_uptime / 60) % 60;
    if (currentMinute != lastMinute) {
        lastMinute = currentMinute;
        for (int i = 0; i < MAX_TIMERS; i++) {
            if (g_timers[i].enabled && g_timers[i].delay_sec == 0 &&
                g_timers[i].hour == currentHour &&
                g_timers[i].minute == currentMinute) {
                printf("[Timer] Timer %d triggered\n", i);
                if (g_timers[i].action == 1) {
                    LED_On();
                } else if (g_timers[i].action == 2) {
                    if (g_ledOn) LED_Off(); else LED_On();
                } else {
                    LED_Off();
                }
                PublishStatus();
            }
        }
    }
}

void Schedule_SetTimer(int id, int enable, int action, int delay_sec, int hour, int minute)
{
    if (id < 0 || id >= MAX_TIMERS) return;
    g_timers[id].enabled = enable;
    g_timers[id].action = action;
    g_timers[id].delay_sec = delay_sec;
    g_timers[id].hour = hour;
    g_timers[id].minute = minute;
    if (delay_sec > 0) {
        g_timers[id].trigger_uptime = g_uptime + delay_sec * 10;
    }
}

void Schedule_SetSlot(int index, int enabled, int sH, int sM, int eH, int eM)
{
    if (index < 0 || index >= MAX_SCHEDULES) return;
    g_schedules[index].enabled = enabled;
    g_schedules[index].startHour = sH;
    g_schedules[index].startMinute = sM;
    g_schedules[index].endHour = eH;
    g_schedules[index].endMinute = eM;

    g_scheduleCount = 0;
    for (int j = 0; j < MAX_SCHEDULES; j++) {
        if (g_schedules[j].enabled) g_scheduleCount++;
    }
}
