#include "gpio_config.h"
#include "scene.h"
#include "led.h"
#include "servo.h"
#include "msmart.h"
#include "udp_server.h"
#include <stdio.h>
#include <string.h>

char g_currentScene[16] = "none";

void Scene_Apply(const char *scene)
{
    if (strcmp(scene, "home") == 0) {
        LED_SetBrightness(100);
        Door_Unlock();
        g_acOn = 1;
        strncpy(g_acMode, "cool", sizeof(g_acMode));
        g_acTargetTemp = 26;
        strncpy(g_currentScene, "home", sizeof(g_currentScene));
        printf("[Scene] HOME mode activated\n");
        PublishStatus();
    } else if (strcmp(scene, "sleep") == 0) {
        LED_SetBrightness(10);
        g_acOn = 1;
        strncpy(g_acMode, "cool", sizeof(g_acMode));
        g_acTargetTemp = 28;
        Door_Lock();
        strncpy(g_currentScene, "sleep", sizeof(g_currentScene));
        printf("[Scene] SLEEP mode activated\n");
        PublishStatus();
    } else if (strcmp(scene, "away") == 0) {
        LED_Off();
        g_acOn = 0;
        Door_Lock();
        strncpy(g_currentScene, "away", sizeof(g_currentScene));
        printf("[Scene] AWAY mode activated\n");
        PublishStatus();
    }
}
