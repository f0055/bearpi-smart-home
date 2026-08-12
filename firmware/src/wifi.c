#include "gpio_config.h"
#include "wifi.h"
#include "lwip/netif.h"
#include "lwip/netifapi.h"
#include "lwip/ip4_addr.h"
#include "lwip/sockets.h"
#include "wifi_device.h"
#include "wifiiot_errno.h"
#include "cmsis_os2.h"
#include <stdio.h>
#include <string.h>

int g_wifiConnected = 0;
static int g_ConnectSuccess = 0;
static int g_staScanSuccess = 0;
static int ssid_count = 0;
static int g_wifiReconnectCounter = 0;

char g_boardIp[20] = "unknown";

static void OnWifiConnectionChangedHandler(int state, WifiLinkedInfo *info)
{
    if (info == NULL) {
        printf("[WiFi] ConnectionChanged: info is null, state=%d\n", state);
        return;
    }
    if (state == WIFI_STATE_AVALIABLE) {
        g_ConnectSuccess = 1;
        g_wifiConnected = 1;
        printf("[WiFi] Connected to %s\n", info->ssid);
    } else {
        g_ConnectSuccess = 0;
        g_wifiConnected = 0;
        printf("[WiFi] Connection failed, state=%d\n", state);
    }
}

static void OnWifiScanStateChangedHandler(int state, int size)
{
    (void)state;
    if (size > 0) {
        ssid_count = size;
        g_staScanSuccess = 1;
    }
}

static void OnHotspotStaJoinHandler(StationInfo *info) { (void)info; }
static void OnHotspotStaLeaveHandler(StationInfo *info) { (void)info; }
static void OnHotspotStateChangedHandler(int state) { (void)state; }

static WifiEvent g_wifiEventHandler = {0};

int WiFiConnect(const char *ssid, const char *psk)
{
    WifiScanInfo *info = NULL;
    unsigned int size = WIFI_SCAN_HOTSPOT_LIMIT;
    struct netif *netif = NULL;
    WifiDeviceConfig apConfig = {0};
    int result = 0;

    osDelay(200);
    printf("[WiFi] Initializing...\n");

    g_wifiEventHandler.OnWifiScanStateChanged = OnWifiScanStateChangedHandler;
    g_wifiEventHandler.OnWifiConnectionChanged = OnWifiConnectionChangedHandler;
    g_wifiEventHandler.OnHotspotStaJoin = OnHotspotStaJoinHandler;
    g_wifiEventHandler.OnHotspotStaLeave = OnHotspotStaLeaveHandler;
    g_wifiEventHandler.OnHotspotStateChanged = OnHotspotStateChangedHandler;

    if (RegisterWifiEvent(&g_wifiEventHandler) != WIFI_SUCCESS) {
        printf("[WiFi] Register event failed\n");
        return -1;
    }

    if (EnableWifi() != WIFI_SUCCESS) {
        printf("[WiFi] Enable failed\n");
        return -1;
    }

    if (IsWifiActive() == 0) {
        printf("[WiFi] WiFi not active\n");
        return -1;
    }

    info = malloc(sizeof(WifiScanInfo) * WIFI_SCAN_HOTSPOT_LIMIT);
    if (info == NULL) {
        printf("[WiFi] Malloc failed\n");
        return -1;
    }

    do {
        ssid_count = 0;
        g_staScanSuccess = 0;
        Scan();
        {
            int scanTimeout = 15;
            while (scanTimeout > 0) {
                sleep(1);
                scanTimeout--;
                if (g_staScanSuccess == 1) break;
            }
        }
        GetScanInfoList(info, &size);
    } while (g_staScanSuccess != 1);

    printf("[WiFi] Scan results:\n");
    for (int i = 0; i < ssid_count; i++) {
        printf("  [%d] %s (rssi:%d)\n", i+1, info[i].ssid, info[i].rssi/100);
    }

    int connected = 0;
    for (int i = 0; i < ssid_count; i++) {
        if (strcmp(ssid, info[i].ssid) == 0) {
            printf("[WiFi] Connecting to %s...\n", ssid);
            strcpy(apConfig.ssid, info[i].ssid);
            strcpy(apConfig.preSharedKey, psk);
            apConfig.securityType = WIFI_SECURITY_TYPE;

            if (AddDeviceConfig(&apConfig, &result) == WIFI_SUCCESS) {
                if (ConnectTo(result) == WIFI_SUCCESS) {
                    int connectTimeout = 15;
                    while (connectTimeout > 0) {
                        sleep(1);
                        connectTimeout--;
                        if (g_ConnectSuccess == 1) {
                            printf("[WiFi] Connection successful\n");
                            connected = 1;
                            break;
                        }
                    }
                }
            }
            break;
        }
    }

    free(info);

    if (!connected) {
        printf("[WiFi] Connection failed\n");
        return -1;
    }

    netif = netifapi_netif_find("wlan0");
    if (netif) {
        dhcp_start(netif);
        printf("[WiFi] Starting DHCP...\n");
        int dhcpTimeout = 30;
        while (dhcp_is_bound(netif) != ERR_OK) {
            sleep(1);
            dhcpTimeout--;
            if (dhcpTimeout <= 0) {
                printf("[WiFi] DHCP timeout\n");
                return -1;
            }
        }
        printf("[WiFi] DHCP successful\n");
        {
            int sock = socket(AF_INET, SOCK_DGRAM, 0);
            if (sock >= 0) {
                struct sockaddr_in dest = {0};
                dest.sin_family = AF_INET;
                dest.sin_port = htons(8888);
                ip4_addr_t target;
                IP4_ADDR(&target, 8, 8, 8, 8);
                dest.sin_addr.s_addr = target.addr;
                if (connect(sock, (struct sockaddr *)&dest, sizeof(dest)) == 0) {
                    struct sockaddr_in local = {0};
                    socklen_t len = sizeof(local);
                    if (getsockname(sock, (struct sockaddr *)&local, &len) == 0) {
                        inet_ntop(AF_INET, &local.sin_addr, g_boardIp, sizeof(g_boardIp));
                        printf("[WiFi] Board IP: %s\n", g_boardIp);
                    }
                }
                closesocket(sock);
            }
        }
    }

    osDelay(200);
    return 0;
}

void WiFi_AutoReconnect(void)
{
    if (g_wifiConnected) {
        g_wifiReconnectCounter = 0;
        return;
    }

    g_wifiReconnectCounter++;
    if (g_wifiReconnectCounter >= WIFI_RECONNECT_SEC * 10) {
        g_wifiReconnectCounter = 0;
        printf("[WiFi] Disconnected, reconnecting...\n");
        if (WiFiConnect(WIFI_SSID, WIFI_PASSWORD) == 0) {
            printf("[WiFi] Reconnected successfully\n");
        }
    }
}
