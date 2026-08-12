#include "gpio_config.h"
#include "state.h"
#include "led.h"
#include "servo.h"
#include "msmart.h"
#include <stdio.h>
#include <string.h>

static StateData g_savedState;
static int g_stateSaved = 0;

void State_Save(void)
{
    g_savedState.magic = STATE_MAGIC;
    g_savedState.ledOn = g_ledOn;
    g_savedState.brightness = g_brightness;
    g_savedState.locked = g_locked;
    g_savedState.acOn = g_acOn;
    strncpy(g_savedState.acMode, g_acMode, sizeof(g_savedState.acMode) - 1);
    g_savedState.acTargetTemp = g_acTargetTemp;
    strncpy(g_savedState.acBrand, g_acBrand, sizeof(g_savedState.acBrand) - 1);
    extern int g_broadcastEnabled;
    g_savedState.broadcastEnabled = g_broadcastEnabled;
    g_stateSaved = 1;
}

void State_Load(void)
{
    if (g_stateSaved && g_savedState.magic == STATE_MAGIC) {
        g_ledOn = g_savedState.ledOn;
        g_brightness = g_savedState.brightness;
        g_locked = g_savedState.locked;
        g_acOn = g_savedState.acOn;
        strncpy(g_acMode, g_savedState.acMode, sizeof(g_acMode) - 1);
        g_acTargetTemp = g_savedState.acTargetTemp;
        strncpy(g_acBrand, g_savedState.acBrand, sizeof(g_acBrand) - 1);
        extern int g_broadcastEnabled;
        g_broadcastEnabled = g_savedState.broadcastEnabled;
        printf("[State] Loaded from backup\n");
        printf("[State] LED=%d, Brightness=%d, Locked=%d, AC=%d\n",
               g_ledOn, g_brightness, g_locked, g_acOn);
    } else {
        printf("[State] Using defaults\n");
    }
}
