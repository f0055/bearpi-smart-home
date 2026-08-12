#ifndef SHT30_H
#define SHT30_H

extern float g_temperature;
extern float g_humidity;

void I2C_Init(void);
void SHT30_Init(void);
void SHT30_Read(void);

#endif
