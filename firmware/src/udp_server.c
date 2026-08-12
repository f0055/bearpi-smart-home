#include "gpio_config.h"
#include "udp_server.h"
#include "led.h"
#include "servo.h"
#include "pir.h"
#include "door.h"
#include "sht30.h"
#include "wifi.h"
#include "msmart.h"
#include "scene.h"
#include "schedule.h"
#include "state.h"
#include "auto_lock.h"
#include "lwip/sockets.h"
#include "cJSON.h"
#include "cmsis_os2.h"
#include <stdio.h>
#include <string.h>

int g_udpSocket = -1;
struct sockaddr_in g_appAddr;
int g_appAddrValid = 0;
int g_broadcastEnabled = 1;
unsigned long g_uptime = 0;
int g_stateDirty = 0;

void UDP_SendMessage(const char *message)
{
    if (g_udpSocket < 0 || !g_appAddrValid) return;
    sendto(g_udpSocket, message, strlen(message), 0,
           (struct sockaddr *)&g_appAddr, sizeof(g_appAddr));
}

void PublishStatus(void)
{
    char status[800];
    snprintf(status, sizeof(status),
             "{\"status\":\"%s\",\"brightness\":%d,\"uptime\":%lu,\"locked\":%s,\"pir\":%s,\"door_open\":%s,\"auto_unlock\":%s,\"temperature\":%.1f,\"humidity\":%.1f,\"ac_on\":%s,\"ac_mode\":\"%s\",\"ac_target_temp\":%d,\"ac_brand\":\"%s\",\"broadcast\":%s,\"scene\":\"%s\",\"midea_found\":%s,\"midea_ip\":\"%s\",\"midea_paired\":%s,\"midea_device_id\":%d,\"time\":\"%02d:%02d\",\"schedule_count\":%d}",
             g_ledOn ? "on" : "off", g_brightness, g_uptime,
             g_locked ? "true" : "false",
             g_pir ? "true" : "false",
             g_door_open ? "true" : "false",
             g_autoUnlockEnabled ? "true" : "false",
             g_temperature, g_humidity,
             g_acOn ? "true" : "false",
             g_acMode, g_acTargetTemp, g_acBrand,
             g_broadcastEnabled ? "true" : "false",
             g_currentScene,
             g_mideaFound ? "true" : "false",
             g_mideaIp,
             g_mideaPaired ? "true" : "false",
             g_mideaDeviceId,
             g_currentHour, g_currentMinute,
             g_scheduleCount);

    UDP_SendMessage(status);
    g_stateDirty = 1;
}

static void ProcessBrightness(cJSON *root)
{
    cJSON *b = cJSON_GetObjectItem(root, "brightness");
    if (b == NULL) b = cJSON_GetObjectItem(root, "value");
    if (b == NULL) b = cJSON_GetObjectItem(root, "level");
    if (b == NULL) return;

    int bval = -1;
    if (cJSON_IsNumber(b)) {
        bval = b->valueint;
    } else if (cJSON_IsString(b)) {
        bval = atoi(b->valuestring);
    }
    if (bval >= 0 && bval <= 100) {
        LED_SetBrightness(bval);
        PublishStatus();
    }
}

static void UDP_ProcessMessage(const char *message, struct sockaddr_in *fromAddr)
{
    g_appAddr = *fromAddr;
    g_appAddrValid = 1;

    cJSON *root = cJSON_Parse(message);
    if (root == NULL) {
        printf("[UDP] JSON parse error\n");
        return;
    }

    cJSON *action = cJSON_GetObjectItem(root, "action");
    if (action && cJSON_IsString(action)) {
        char actBuf[32];
        strncpy(actBuf, action->valuestring, sizeof(actBuf) - 1);
        actBuf[sizeof(actBuf) - 1] = '\0';
        for (int i = 0; actBuf[i]; i++) {
            if (actBuf[i] >= 'A' && actBuf[i] <= 'Z') actBuf[i] += 32;
        }
        if (strcmp(actBuf, "on") == 0) {
            LED_On();
        } else if (strcmp(actBuf, "off") == 0) {
            LED_Off();
        } else if (strcmp(actBuf, "status") == 0) {
            static unsigned long lastStatusTime = 0;
            if (g_uptime - lastStatusTime >= 10) {
                lastStatusTime = g_uptime;
                PublishStatus();
            }
        } else if (strcmp(actBuf, "lock") == 0) {
            Door_Lock();
            PublishStatus();
        } else if (strcmp(actBuf, "unlock") == 0) {
            Door_Unlock();
            PublishStatus();
        } else if (strcmp(actBuf, "ac_on") == 0) {
            g_acOn = 1;
            printf("[AC] Turned ON, mode=%s, target=%dC\n", g_acMode, g_acTargetTemp);
            if (g_mideaPaired) {
                Midea_Control(1, g_acTargetTemp, g_acMode);
            }
            PublishStatus();
        } else if (strcmp(actBuf, "ac_off") == 0) {
            g_acOn = 0;
            printf("[AC] Turned OFF\n");
            if (g_mideaPaired) {
                Midea_Control(0, g_acTargetTemp, g_acMode);
            }
            PublishStatus();
        } else if (strcmp(actBuf, "brightness") == 0 ||
                   strcmp(actBuf, "setbrightness") == 0 ||
                    strcmp(actBuf, "dimmer") == 0) {
            ProcessBrightness(root);
            PublishStatus();
        }
    }

    cJSON *acMode = cJSON_GetObjectItem(root, "ac_mode");
    if (acMode && cJSON_IsString(acMode)) {
        strncpy(g_acMode, acMode->valuestring, sizeof(g_acMode) - 1);
        g_acMode[sizeof(g_acMode) - 1] = '\0';
        printf("[AC] Mode set to %s\n", g_acMode);
        if (g_acOn && g_mideaPaired) {
            Midea_Control(1, g_acTargetTemp, g_acMode);
        }
        PublishStatus();
    }

    cJSON *acTargetTemp = cJSON_GetObjectItem(root, "ac_target_temp");
    if (acTargetTemp && cJSON_IsNumber(acTargetTemp)) {
        g_acTargetTemp = acTargetTemp->valueint;
        if (g_acTargetTemp < 16) g_acTargetTemp = 16;
        if (g_acTargetTemp > 30) g_acTargetTemp = 30;
        printf("[AC] Target temp set to %dC\n", g_acTargetTemp);
        if (g_acOn && g_mideaPaired) {
            Midea_Control(1, g_acTargetTemp, g_acMode);
        }
        PublishStatus();
    }

    cJSON *acBrand = cJSON_GetObjectItem(root, "ac_brand");
    if (acBrand && cJSON_IsString(acBrand)) {
        strncpy(g_acBrand, acBrand->valuestring, sizeof(g_acBrand) - 1);
        g_acBrand[sizeof(g_acBrand) - 1] = '\0';
        printf("[AC] Brand set to %s\n", g_acBrand);
        PublishStatus();
    }

    cJSON *brightness = cJSON_GetObjectItem(root, "brightness");
    if (brightness) {
        ProcessBrightness(root);
    }

    cJSON *setTimer = cJSON_GetObjectItem(root, "set_timer");
    if (setTimer && cJSON_IsObject(setTimer)) {
        cJSON *timerId = cJSON_GetObjectItem(setTimer, "timer_id");
        cJSON *timerEnable = cJSON_GetObjectItem(setTimer, "enable");
        cJSON *timerAction = cJSON_GetObjectItem(setTimer, "action");

        if (timerId && timerEnable && timerAction) {
            int id = timerId->valueint;
            if (id >= 0 && id < MAX_TIMERS) {
                int action = (strcmp(timerAction->valuestring, "on") == 0) ? 1 :
                             (strcmp(timerAction->valuestring, "toggle") == 0) ? 2 : 0;
                int delay_sec = 0;
                int hour = 0, minute = 0;

                cJSON *delaySec = cJSON_GetObjectItem(setTimer, "delay_sec");
                if (delaySec && cJSON_IsNumber(delaySec) && delaySec->valueint > 0) {
                    delay_sec = delaySec->valueint;
                } else {
                    cJSON *timerHour = cJSON_GetObjectItem(setTimer, "hour");
                    cJSON *timerMinute = cJSON_GetObjectItem(setTimer, "minute");
                    if (timerHour && timerMinute) {
                        hour = timerHour->valueint;
                        minute = timerMinute->valueint;
                        unsigned long target = (hour * 3600 + minute * 60) * 10;
                        if (target > g_uptime) {
                            delay_sec = (target - g_uptime) / 10;
                        }
                    }
                }
                Schedule_SetTimer(id, timerEnable->valueint, action, delay_sec, hour, minute);
            }
        }
    }

    cJSON *scene = cJSON_GetObjectItem(root, "scene");
    if (scene && cJSON_IsString(scene)) {
        Scene_Apply(scene->valuestring);
    }

    cJSON *autoUnlock = cJSON_GetObjectItem(root, "auto_unlock");
    if (autoUnlock && cJSON_IsBool(autoUnlock)) {
        AutoLock_SetEnabled(cJSON_IsTrue(autoUnlock) ? 1 : 0);
        PublishStatus();
    }

    cJSON *timeSync = cJSON_GetObjectItem(root, "time_sync");
    if (timeSync && cJSON_IsObject(timeSync)) {
        cJSON *hour = cJSON_GetObjectItem(timeSync, "hour");
        cJSON *minute = cJSON_GetObjectItem(timeSync, "minute");
        if (hour && cJSON_IsNumber(hour) && minute && cJSON_IsNumber(minute)) {
            g_currentHour = hour->valueint;
            g_currentMinute = minute->valueint;
            printf("[Time] Sync: %02d:%02d\n", g_currentHour, g_currentMinute);
        }
    }

    cJSON *setSchedule = cJSON_GetObjectItem(root, "set_schedule");
    if (setSchedule && cJSON_IsObject(setSchedule)) {
        cJSON *idx = cJSON_GetObjectItem(setSchedule, "index");
        cJSON *enable = cJSON_GetObjectItem(setSchedule, "enabled");
        cJSON *sH = cJSON_GetObjectItem(setSchedule, "start_hour");
        cJSON *sM = cJSON_GetObjectItem(setSchedule, "start_minute");
        cJSON *eH = cJSON_GetObjectItem(setSchedule, "end_hour");
        cJSON *eM = cJSON_GetObjectItem(setSchedule, "end_minute");

        if (idx && cJSON_IsNumber(idx)) {
            int i = idx->valueint;
            if (i >= 0 && i < MAX_SCHEDULES) {
                Schedule_SetSlot(i,
                    (enable && cJSON_IsTrue(enable)) ? 1 : 0,
                    sH && cJSON_IsNumber(sH) ? sH->valueint : 0,
                    sM && cJSON_IsNumber(sM) ? sM->valueint : 0,
                    eH && cJSON_IsNumber(eH) ? eH->valueint : 0,
                    eM && cJSON_IsNumber(eM) ? eM->valueint : 0);
                printf("[Schedule] Set slot %d: %s %02d:%02d-%02d:%02d\n",
                       i, g_schedules[i].enabled ? "ON" : "OFF",
                       g_schedules[i].startHour, g_schedules[i].startMinute,
                       g_schedules[i].endHour, g_schedules[i].endMinute);
                PublishStatus();
            }
        }
    }

    cJSON *mideaDiscover = cJSON_GetObjectItem(root, "midea_discover");
    if (mideaDiscover && cJSON_IsTrue(mideaDiscover)) {
        printf("[Midea] Starting discovery...\n");
        Midea_Discover();
        PublishStatus();
    }

    cJSON *mideaToken = cJSON_GetObjectItem(root, "midea_token");
    if (mideaToken && cJSON_IsString(mideaToken)) {
        printf("[Midea] Received token, pairing...\n");
        Midea_Pair(mideaToken->valuestring);
        PublishStatus();
    }

    cJSON *mideaControl = cJSON_GetObjectItem(root, "midea_control");
    if (mideaControl && cJSON_IsObject(mideaControl)) {
        cJSON *mOn = cJSON_GetObjectItem(mideaControl, "on");
        cJSON *mTemp = cJSON_GetObjectItem(mideaControl, "temp");
        cJSON *mMode = cJSON_GetObjectItem(mideaControl, "mode");

        int on = mOn && cJSON_IsTrue(mOn);
        int temp = mTemp && cJSON_IsNumber(mTemp) ? mTemp->valueint : 26;
        const char *mode = (mMode && cJSON_IsString(mMode)) ? mMode->valuestring : "cool";

        Midea_Control(on, temp, mode);
        PublishStatus();
    }

    cJSON *broadcast = cJSON_GetObjectItem(root, "broadcast");
    if (broadcast && cJSON_IsBool(broadcast)) {
        g_broadcastEnabled = cJSON_IsTrue(broadcast) ? 1 : 0;
        printf("[Broadcast] %s\n", g_broadcastEnabled ? "Enabled" : "Disabled");
        PublishStatus();
    }

    cJSON_Delete(root);
}

void UDP_Recv_Task(void)
{
    char buffer[UDP_BUFFER_SIZE];
    struct sockaddr_in serverAddr, fromAddr;
    socklen_t fromAddrLen = sizeof(fromAddr);

    g_udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (g_udpSocket < 0) {
        printf("[UDP] Create socket failed\n");
        return;
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(UDP_PORT);

    if (bind(g_udpSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        printf("[UDP] Bind failed\n");
        closesocket(g_udpSocket);
        return;
    }

    printf("[UDP] Listening on port %d\n", UDP_PORT);

    while (1) {
        int len = recvfrom(g_udpSocket, buffer, UDP_BUFFER_SIZE - 1, 0,
                          (struct sockaddr *)&fromAddr, &fromAddrLen);
        if (len > 0) {
            buffer[len] = '\0';
            UDP_ProcessMessage(buffer, &fromAddr);
        }
    }
}

void UDP_Broadcast_Task(void)
{
    int broadcastSock;
    struct sockaddr_in broadcastAddr;

    broadcastSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (broadcastSock < 0) {
        printf("[Broadcast] Create socket failed\n");
        return;
    }

    int enable = 1;
    setsockopt(broadcastSock, SOL_SOCKET, SO_BROADCAST, &enable, sizeof(enable));

    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_port = htons(BROADCAST_PORT);
    broadcastAddr.sin_addr.s_addr = inet_addr("255.255.255.255");

    while (strcmp(g_boardIp, "unknown") == 0) {
        sleep(1);
    }
    printf("[Broadcast] Board IP: %s, discovery broadcasting every %ds\n", g_boardIp, BROADCAST_INTERVAL);

    while (1) {
        if (g_broadcastEnabled) {
            char msg[256];
            snprintf(msg, sizeof(msg),
                     "{\"device\":\"BearPi-HM_Nano\",\"ip\":\"%s\",\"status\":\"online\",\"locked\":%s,\"temperature\":%.1f,\"humidity\":%.1f}",
                     g_boardIp,
                     g_locked ? "true" : "false",
                     g_temperature, g_humidity);

            sendto(broadcastSock, msg, strlen(msg), 0,
                   (struct sockaddr *)&broadcastAddr, sizeof(broadcastAddr));
        }
        sleep(BROADCAST_INTERVAL);
    }
}
