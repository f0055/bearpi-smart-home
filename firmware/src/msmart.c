#include "gpio_config.h"
#include "msmart.h"
#include "aes.h"
#include "md5.h"
#include "udp_server.h"
#include "wifi.h"
#include "lwip/sockets.h"
#include <stdio.h>
#include <string.h>

int g_acOn = 0;
char g_acMode[8] = "cool";
int g_acTargetTemp = 26;
char g_acBrand[16] = "midea";

int g_mideaFound = 0;
char g_mideaIp[20] = "";
static unsigned char g_mideaKey[16] = {0};
int g_mideaPaired = 0;
static unsigned char g_mideaToken[16] = {0};
unsigned short g_mideaDeviceId = 0;
static unsigned char g_mideaSeq = 0x5c;

static void Midea_DeriveKey(const unsigned char *token, unsigned short deviceId, unsigned char key[16])
{
    unsigned char material[24];
    unsigned char devId[2] = { (unsigned char)(deviceId & 0xff), (unsigned char)((deviceId >> 8) & 0xff) };
    memcpy(material + 0,  token,        4);
    memcpy(material + 4,  devId,        2);
    memcpy(material + 6,  token + 4,    4);
    memcpy(material + 10, devId,        2);
    memcpy(material + 12, token + 8,    4);
    memcpy(material + 16, devId,        2);
    memcpy(material + 18, token + 12,   4);
    memcpy(material + 22, devId,        2);
    MD5(material, 24, key);
}

static int Midea_SendRaw(const unsigned char *data, int len)
{
    int sock;
    struct sockaddr_in destAddr;

    if (!g_mideaFound) { printf("[Midea] AC not found, discovery first\n"); return -1; }

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) { printf("[Midea] Create socket failed\n"); return -1; }

    struct timeval timeout;
    timeout.tv_sec = 3; timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    memset(&destAddr, 0, sizeof(destAddr));
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(MIDEA_AC_PORT);
    destAddr.sin_addr.s_addr = inet_addr(g_mideaIp);

    int ret = sendto(sock, data, len, 0, (struct sockaddr *)&destAddr, sizeof(destAddr));
    if (ret < 0) { printf("[Midea] Send failed\n"); closesocket(sock); return -1; }

    unsigned char resp[256];
    struct sockaddr_in fromAddr;
    socklen_t fromLen = sizeof(fromAddr);
    int r = recvfrom(sock, resp, sizeof(resp), 0, (struct sockaddr *)&fromAddr, &fromLen);
    if (r > 0) printf("[Midea] AC response %d bytes\n", r);
    closesocket(sock);
    printf("[Midea] Sent %d bytes to %s\n", len, g_mideaIp);
    return 0;
}

int Midea_Discover(void)
{
    int sock;
    struct sockaddr_in destAddr;
    struct sockaddr_in fromAddr;
    socklen_t fromLen = sizeof(fromAddr);
    unsigned char recvBuf[256];

    unsigned char disc[40];
    memset(disc, 0, sizeof(disc));
    disc[0] = 0x5a; disc[1] = 0x5a; disc[2] = 0x01; disc[3] = 0x11;
    disc[4] = 0x48; disc[5] = 0x00; disc[6] = g_mideaSeq++;
    int cs = 0;
    for (int i = 0; i < 39; i++) cs += disc[i];
    disc[39] = (unsigned char)(cs & 0xff);

    printf("[Midea] Scanning for AC on port %d...\n", MIDEA_AC_PORT);

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) { printf("[Midea] Create socket failed\n"); return -1; }

    int broadcast = 1;
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    struct timeval timeout;
    timeout.tv_sec = 5; timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    memset(&destAddr, 0, sizeof(destAddr));
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(MIDEA_AC_PORT);
    destAddr.sin_addr.s_addr = inet_addr("255.255.255.255");
    sendto(sock, disc, sizeof(disc), 0, (struct sockaddr *)&destAddr, sizeof(destAddr));
    printf("[Midea] Broadcast sent to 255.255.255.255:%d\n", MIDEA_AC_PORT);

    int ret = recvfrom(sock, recvBuf, sizeof(recvBuf), 0, (struct sockaddr *)&fromAddr, &fromLen);
    if (ret < 0) {
        printf("[Midea] Broadcast no response, trying 192.168.1.6...\n");
        destAddr.sin_addr.s_addr = inet_addr("192.168.1.6");
        sendto(sock, disc, sizeof(disc), 0, (struct sockaddr *)&destAddr, sizeof(destAddr));
        timeout.tv_sec = 3;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        ret = recvfrom(sock, recvBuf, sizeof(recvBuf), 0, (struct sockaddr *)&fromAddr, &fromLen);
    }

    if (ret > 0) {
        inet_ntop(AF_INET, &fromAddr.sin_addr, g_mideaIp, sizeof(g_mideaIp));
        g_mideaFound = 1;
        if (ret >= 10) {
            g_mideaDeviceId = (unsigned short)(recvBuf[8] | (recvBuf[9] << 8));
        }
        printf("[Midea] Found AC at %s (port %d), deviceId=0x%04X, respLen=%d\n",
               g_mideaIp, MIDEA_AC_PORT, g_mideaDeviceId, ret);
        closesocket(sock);
        if (g_mideaPaired && g_mideaToken[0] != 0) {
            Midea_DeriveKey(g_mideaToken, g_mideaDeviceId, g_mideaKey);
            printf("[Midea] Key derived from token (deviceId=0x%04X)\n", g_mideaDeviceId);
        }
        return 0;
    }

    printf("[Midea] No AC found (timeout)\n");
    closesocket(sock);
    return -1;
}

int Midea_Pair(const char *tokenHex)
{
    int len = (int)strlen(tokenHex);
    if (len < 32) { printf("[Midea] Token too short (need 32 hex chars)\n"); return -1; }
    for (int i = 0; i < 16; i++) {
        unsigned int b;
        char tmp[3] = { tokenHex[i*2], tokenHex[i*2+1], 0 };
        sscanf(tmp, "%02x", &b);
        g_mideaToken[i] = (unsigned char)b;
    }
    g_mideaPaired = 1;
    if (g_mideaFound) {
        Midea_DeriveKey(g_mideaToken, g_mideaDeviceId, g_mideaKey);
        printf("[Midea] Paired. deviceId=0x%04X, key derived\n", g_mideaDeviceId);
    } else {
        printf("[Midea] Token stored, run discovery to derive key\n");
    }
    return 0;
}

int Midea_Control(int on, int temp, const char *mode)
{
    if (!g_mideaPaired) { printf("[Midea] Not paired, send token first\n"); return -1; }

    unsigned char plain[16];
    memset(plain, 0, sizeof(plain));
    plain[0] = 0x01;
    plain[1] = 0x02;
    int modeByte = 1;
    if (strcmp(mode, "cool") == 0) modeByte = 1;
    else if (strcmp(mode, "dry") == 0) modeByte = 2;
    else if (strcmp(mode, "fan") == 0) modeByte = 3;
    else if (strcmp(mode, "heat") == 0) modeByte = 4;
    plain[2] = (unsigned char)((on ? 0x01 : 0x00) | (modeByte << 4));
    plain[3] = (unsigned char)temp;

    unsigned char cipher[16];
    AES_EncryptBlock(plain, g_mideaKey, cipher);

    unsigned char pkt[40];
    memset(pkt, 0, sizeof(pkt));
    pkt[0] = 0x5a; pkt[1] = 0x5a; pkt[2] = 0x01; pkt[3] = 0x11;
    pkt[4] = 0x48; pkt[5] = 0x00; pkt[6] = g_mideaSeq++;
    pkt[7] = 0x00;
    pkt[8] = (unsigned char)(g_mideaDeviceId & 0xff);
    pkt[9] = (unsigned char)((g_mideaDeviceId >> 8) & 0xff);
    memcpy(pkt + 10, cipher, 16);

    int cs = 0;
    for (int i = 0; i < 39; i++) cs += pkt[i];
    pkt[39] = (unsigned char)(cs & 0xff);

    if (Midea_SendRaw(pkt, sizeof(pkt)) == 0) {
        printf("[Midea] Control sent: on=%d temp=%d mode=%s (encrypted)\n", on, temp, mode);
        return 0;
    }
    return -1;
}
