#ifndef STATE_H
#define STATE_H

typedef struct {
    unsigned int magic;
    int ledOn;
    int brightness;
    int locked;
    int acOn;
    char acMode[8];
    int acTargetTemp;
    char acBrand[16];
    int broadcastEnabled;
} StateData;

void State_Save(void);
void State_Load(void);

#endif
