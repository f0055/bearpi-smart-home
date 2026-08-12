#ifndef MD5_H
#define MD5_H

typedef struct {
    unsigned int state[4];
    unsigned int count[2];
    unsigned char buf[64];
} MD5_CTX;

void MD5_Init(MD5_CTX *c);
void MD5_Update(MD5_CTX *c, const unsigned char *in, unsigned int len);
void MD5_Final(MD5_CTX *c, unsigned char digest[16]);
void MD5(const unsigned char *data, unsigned int len, unsigned char digest[16]);

#endif
