#include "gpio_config.h"
#include "md5.h"
#include <string.h>

#define MD5_H(x,y,z) (x ^ y ^ z)
#define MD5_I(x,y,z) (y ^ (x | ~z))
#define MD5_ROL(x,n) ((x << n) | (x >> (32 - n)))
#define MD5_STEP(f,a,b,c,d,x,s,ac) { (a) += f((b),(c),(d)) + (x) + (unsigned int)(ac); \
    (a) = MD5_ROL((a),(s)); (a) += (b); }
void MD5_Init(MD5_CTX *c)
{
    c->state[0] = 0x67452301; c->state[1] = 0xefcdab89;
    c->state[2] = 0x98badcfe; c->state[3] = 0x10325476;
    c->count[0] = c->count[1] = 0;
}
static void MD5_Transform(unsigned int state[4], const unsigned char b[64])
{
    unsigned int a = state[0], c = state[1], d = state[2], e = state[3], x[16];
    for (int i = 0; i < 16; i++)
        x[i] = (unsigned int)b[i*4] | ((unsigned int)b[i*4+1]<<8) |
               ((unsigned int)b[i*4+2]<<16) | ((unsigned int)b[i*4+3]<<24);
    MD5_STEP(MD5_F, a,c,d,e, x[0], 7,0xd76aa478); MD5_STEP(MD5_F, e,a,c,d, x[1],12,0xe8c7b756);
    MD5_STEP(MD5_F, d,e,a,c, x[2],17,0x242070db); MD5_STEP(MD5_F, c,d,e,a, x[3],22,0xc1bdceee);
    MD5_STEP(MD5_F, a,c,d,e, x[4], 7,0xf57c0faf); MD5_STEP(MD5_F, e,a,c,d, x[5],12,0x4787c62a);
    MD5_STEP(MD5_F, d,e,a,c, x[6],17,0xa8304613); MD5_STEP(MD5_F, c,d,e,a, x[7],22,0xfd469501);
    MD5_STEP(MD5_F, a,c,d,e, x[8], 7,0x698098d8); MD5_STEP(MD5_F, e,a,c,d, x[9],12,0x8b44f7af);
    MD5_STEP(MD5_F, d,e,a,c,x[10],17,0xffff5bb1); MD5_STEP(MD5_F, c,d,e,a,x[11],22,0x895cd7be);
    MD5_STEP(MD5_F, a,c,d,e,x[12], 7,0x6b901122); MD5_STEP(MD5_F, e,a,c,d,x[13],12,0xfd987193);
    MD5_STEP(MD5_F, d,e,a,c,x[14],17,0xa679438e); MD5_STEP(MD5_F, c,d,e,a,x[15],22,0x49b40821);
    MD5_STEP(MD5_G, a,c,d,e, x[1], 5,0xf61e2562); MD5_STEP(MD5_G, e,a,c,d, x[6], 9,0xc040b340);
    MD5_STEP(MD5_G, d,e,a,c,x[11],14,0x265e5a51); MD5_STEP(MD5_G, c,d,e,a, x[0],20,0xe9b6c7aa);
    MD5_STEP(MD5_G, a,c,d,e, x[5], 5,0xd62f105d); MD5_STEP(MD5_G, e,a,c,d,x[10], 9,0x02441453);
    MD5_STEP(MD5_G, d,e,a,c,x[15],14,0xd8a1e681); MD5_STEP(MD5_G, c,d,e,a, x[4],20,0xe7d3fbc8);
    MD5_STEP(MD5_G, a,c,d,e, x[9], 5,0x21e1cde6); MD5_STEP(MD5_G, e,a,c,d,x[14], 9,0xc33707d6);
    MD5_STEP(MD5_G, d,e,a,c, x[3],14,0xf4d50d87); MD5_STEP(MD5_G, c,d,e,a, x[8],20,0x455a14ed);
    MD5_STEP(MD5_G, a,c,d,e,x[13], 5,0xa9e3e905); MD5_STEP(MD5_G, e,a,c,d, x[2], 9,0xfcefa3f8);
    MD5_STEP(MD5_G, d,e,a,c, x[7],14,0x676f02d9); MD5_STEP(MD5_G, c,d,e,a,x[12],20,0x8d2a4c8a);
    MD5_STEP(MD5_H, a,c,d,e, x[5], 4,0xfffa3942); MD5_STEP(MD5_H, e,a,c,d, x[8],11,0x8771f681);
    MD5_STEP(MD5_H, d,e,a,c,x[11],16,0x6d9d6122); MD5_STEP(MD5_H, c,d,e,a,x[14],23,0xfde5380c);
    MD5_STEP(MD5_H, a,c,d,e, x[1], 4,0xa4beea44); MD5_STEP(MD5_H, e,a,c,d, x[4],11,0x4bdecfa9);
    MD5_STEP(MD5_H, d,e,a,c, x[7],16,0xf6bb4b60); MD5_STEP(MD5_H, c,d,e,a,x[10],23,0xbebfbc70);
    MD5_STEP(MD5_H, a,c,d,e,x[13], 4,0x289b7ec6); MD5_STEP(MD5_H, e,a,c,d, x[0],11,0xeaa127fa);
    MD5_STEP(MD5_H, d,e,a,c, x[3],16,0xd4ef3085); MD5_STEP(MD5_H, c,d,e,a, x[6],23,0x04881d05);
    MD5_STEP(MD5_H, a,c,d,e, x[9], 4,0xd9d4d039); MD5_STEP(MD5_H, e,a,c,d,x[12],11,0xe6db99e5);
    MD5_STEP(MD5_H, d,e,a,c,x[15],16,0x1fa27cf8); MD5_STEP(MD5_H, c,d,e,a, x[2],23,0xc4ac5665);
    MD5_STEP(MD5_I, a,c,d,e, x[0], 6,0xf4292244); MD5_STEP(MD5_I, e,a,c,d, x[7],10,0x432aff97);
    MD5_STEP(MD5_I, d,e,a,c,x[14],15,0xab9423a7); MD5_STEP(MD5_I, c,d,e,a, x[5],21,0xfc93a039);
    MD5_STEP(MD5_I, a,c,d,e,x[12], 6,0x655b59c3); MD5_STEP(MD5_I, e,a,c,d, x[3],10,0x8f0ccc92);
    MD5_STEP(MD5_I, d,e,a,c,x[10],15,0xffeff47d); MD5_STEP(MD5_I, c,d,e,a, x[1],21,0x85845dd1);
    MD5_STEP(MD5_I, a,c,d,e, x[8], 6,0x6fa87e4f); MD5_STEP(MD5_I, e,a,c,d,x[15],10,0xfe2ce6e0);
    MD5_STEP(MD5_I, d,e,a,c, x[6],15,0xa3014314); MD5_STEP(MD5_I, c,d,e,a,x[13],21,0x4e0811a1);
    MD5_STEP(MD5_I, a,c,d,e, x[4], 6,0xf7537e82); MD5_STEP(MD5_I, e,a,c,d,x[11],10,0xbd3af235);
    MD5_STEP(MD5_I, d,e,a,c, x[2],15,0x2ad7d2bb); MD5_STEP(MD5_I, c,d,e,a, x[9],21,0xeb86d391);
    state[0] += a; state[1] += c; state[2] += d; state[3] += e;
}
void MD5_Update(MD5_CTX *c, const unsigned char *in, unsigned int len)
{
    unsigned int have = (c->count[0] >> 3) & 0x3f;
    c->count[0] += (len << 3);
    if (c->count[0] < (len << 3)) c->count[1]++;
    c->count[1] += (len >> 29);
    unsigned int need = 64 - have;
    unsigned int i = 0;
    if (len >= need) {
        memcpy(c->buf + have, in, need);
        MD5_Transform(c->state, c->buf);
        for (i = need; i + 64 <= len; i += 64) MD5_Transform(c->state, in + i);
        have = 0;
    }
    if (len > i) memcpy(c->buf + have, in + i, len - i);
}
void MD5_Final(MD5_CTX *c, unsigned char digest[16])
{
    unsigned int lo = c->count[0] << 3;
    unsigned int hi = (c->count[1] << 3) | (c->count[0] >> 29);
    unsigned char bits[8];
    bits[0] = (unsigned char)(lo & 0xff);
    bits[1] = (unsigned char)((lo >> 8) & 0xff);
    bits[2] = (unsigned char)((lo >> 16) & 0xff);
    bits[3] = (unsigned char)((lo >> 24) & 0xff);
    bits[4] = (unsigned char)(hi & 0xff);
    bits[5] = (unsigned char)((hi >> 8) & 0xff);
    bits[6] = (unsigned char)((hi >> 16) & 0xff);
    bits[7] = (unsigned char)((hi >> 24) & 0xff);

    unsigned int have = (c->count[0] >> 3) & 0x3f;
    unsigned char pad = (have < 56) ? (56 - have) : (120 - have);
    unsigned char p = 0x80;
    MD5_Update(c, &p, 1);
    unsigned char zero = 0;
    for (unsigned int k = 1; k < pad; k++) MD5_Update(c, &zero, 1);
    MD5_Update(c, bits, 8);
    for (int i = 0; i < 4; i++) {
        digest[i*4+0] = (unsigned char)(c->state[i] & 0xff);
        digest[i*4+1] = (unsigned char)((c->state[i] >> 8) & 0xff);
        digest[i*4+2] = (unsigned char)((c->state[i] >> 16) & 0xff);
        digest[i*4+3] = (unsigned char)((c->state[i] >> 24) & 0xff);
    }
}
void MD5(const unsigned char *data, unsigned int len, unsigned char digest[16])
{
    MD5_CTX c;
    MD5_Init(&c);
    MD5_Update(&c, data, len);
    MD5_Final(&c, digest);
}

/* Token+DeviceID -> 16瀛楄妭閫氫俊瀵嗛挜锛坢idealocal绠楁硶锛?*/