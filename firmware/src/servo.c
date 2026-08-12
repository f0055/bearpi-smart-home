#include "gpio_config.h"
#include "servo.h"
#include "udp_server.h"
#include "hi_time.h"
#include "hi_isr.h"
#include <stdio.h>

int g_locked = 1;

void Servo_Init(void)
{
    IoSetFunc(SERVO_GPIO, WIFI_IOT_IO_FUNC_GPIO_13_GPIO);
    GpioSetDir(SERVO_GPIO, WIFI_IOT_GPIO_DIR_OUT);
    GpioSetOutputVal(SERVO_GPIO, WIFI_IOT_GPIO_VALUE0);
    printf("[Servo] Initialized (GPIO_13)\n");
}

void Servo_Pulse(unsigned int pulse_us)
{
    hi_u32 irqStatus = hi_int_lock();
    GpioSetOutputVal(SERVO_GPIO, WIFI_IOT_GPIO_VALUE1);
    hi_udelay(pulse_us);
    GpioSetOutputVal(SERVO_GPIO, WIFI_IOT_GPIO_VALUE0);
    hi_int_restore(irqStatus);
    hi_udelay(20000 - pulse_us);
}

void Servo_SetAngle(unsigned int pulse)
{
    int i;
    for (i = 0; i < SERVO_MOVE_PULSES; i++) {
        Servo_Pulse(pulse);
    }
}

void Door_Lock(void)
{
    if (g_locked) return;
    g_locked = 1;
    PublishStatus();
    Servo_SetAngle(SERVO_LOCK_PULSE);
    printf("[Door] Locked\n");
}

void Door_Unlock(void)
{
    if (!g_locked) return;
    g_locked = 0;
    PublishStatus();
    Servo_SetAngle(SERVO_UNLOCK_PULSE);
    printf("[Door] Unlocked\n");
}
