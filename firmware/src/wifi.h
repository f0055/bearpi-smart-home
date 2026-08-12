#ifndef WIFI_H
#define WIFI_H

extern int g_wifiConnected;
extern char g_boardIp[];

int WiFiConnect(const char *ssid, const char *psk);
void WiFi_AutoReconnect(void);

#endif
