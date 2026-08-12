#ifndef SERVO_H
#define SERVO_H

extern int g_locked;

void Servo_Init(void);
void Servo_Pulse(unsigned int pulse_us);
void Servo_SetAngle(unsigned int pulse);
void Door_Lock(void);
void Door_Unlock(void);

#endif
