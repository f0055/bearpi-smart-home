#ifndef LED_H
#define LED_H

extern int g_ledOn;
extern int g_brightness;

void LED_Init(void);
void LED_On(void);
void LED_Off(void);
void LED_SetBrightness(int brightness);

#endif
