#ifndef UDP_SERVER_H
#define UDP_SERVER_H

#include "lwip/sockets.h"

extern int g_udpSocket;
extern struct sockaddr_in g_appAddr;
extern int g_appAddrValid;
extern int g_broadcastEnabled;
extern unsigned long g_uptime;
extern int g_stateDirty;

void UDP_SendMessage(const char *message);
void PublishStatus(void);
void UDP_Recv_Task(void);
void UDP_Broadcast_Task(void);

#endif
