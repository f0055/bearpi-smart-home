#ifndef MSMART_H
#define MSMART_H

extern int g_acOn;
extern char g_acMode[];
extern int g_acTargetTemp;
extern char g_acBrand[];
extern int g_mideaFound;
extern char g_mideaIp[];
extern int g_mideaPaired;
extern unsigned short g_mideaDeviceId;

int Midea_Discover(void);
int Midea_Pair(const char *tokenHex);
int Midea_Control(int on, int temp, const char *mode);

#endif
