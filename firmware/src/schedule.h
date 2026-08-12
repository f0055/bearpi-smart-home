#ifndef SCHEDULE_H
#define SCHEDULE_H

typedef struct {
    int enabled;
    int hour;
    int minute;
    int action;
    int delay_sec;
    unsigned long trigger_uptime;
} Timer;

typedef struct {
    int enabled;
    int startHour;
    int startMinute;
    int endHour;
    int endMinute;
} TimeSchedule;

extern Timer g_timers[];
extern TimeSchedule g_schedules[];
extern int g_scheduleCount;
extern int g_currentHour;
extern int g_currentMinute;

void Schedule_CheckTimers(void);
void Schedule_SetTimer(int id, int enable, int action, int delay_sec, int hour, int minute);
void Schedule_SetSlot(int index, int enabled, int sH, int sM, int eH, int eM);

#endif
