#ifndef GPIO_CONFIG_H
#define GPIO_CONFIG_H

#include "wifiiot_gpio.h"
#include "wifiiot_gpio_ex.h"
#include "wifiiot_pwm.h"
#include "wifiiot_i2c.h"
#include "wifiiot_i2c_ex.h"

/* WiFi配置 - 请修改为你自己的WiFi名称和密码 */
#define WIFI_SSID           "YOUR_WIFI_SSID"
#define WIFI_PASSWORD       "YOUR_WIFI_PASSWORD"
#define WIFI_SECURITY_TYPE  WIFI_SEC_TYPE_PSK

/* UDP配置 */
#define UDP_PORT            8888
#define UDP_BUFFER_SIZE     512
#define BROADCAST_PORT      9999
#define BROADCAST_INTERVAL  5

/* 美的空调 mSmart 协议配置 */
#define MIDEA_AC_PORT       6445

/* LED引脚定义 */
#define LED_GPIO            WIFI_IOT_IO_NAME_GPIO_2
#define LED_PWM_PORT        WIFI_IOT_PWM_PORT_PWM2

/* 舵机引脚定义 - GPIO_13 */
#define SERVO_GPIO          WIFI_IOT_IO_NAME_GPIO_13
#define SERVO_LOCK_PULSE    500
#define SERVO_UNLOCK_PULSE  2500
#define SERVO_MOVE_PULSES   13

/* PIR引脚定义 - GPIO_6 (HW-870反射式红外) */
#define PIR_GPIO            WIFI_IOT_IO_NAME_GPIO_6

/* 门磁传感器引脚定义 - GPIO_9 (槽型红外传感器) */
#define DOOR_GPIO           WIFI_IOT_IO_NAME_GPIO_9

/* 本地按键引脚定义 - F1=GPIO_11(控制LED), F2=GPIO_12(控制舵机) */
#define BUTTON_F1_GPIO      WIFI_IOT_IO_NAME_GPIO_11
#define BUTTON_F2_GPIO      WIFI_IOT_IO_NAME_GPIO_12

/* 自动解锁时间配置 */
#define AUTO_UNLOCK_SEC     5
#define AUTO_MODE_START     8
#define AUTO_MODE_END       22
#define WIFI_RECONNECT_SEC  30

/* SHT30温湿度传感器定义 - I2C1 (GPIO_0/1) */
#define SHT30_I2C_IDX       WIFI_IOT_I2C_IDX_1
#define SHT30_ADDR          0x44

/* PWM配置 */
#define PWM_DUTY_MAX        40000

/* 定时器配置 */
#define MAX_TIMERS          5
#define MAX_SCHEDULES       5

/* Flash状态保存地址 */
#define STATE_MAGIC         0x48423139

#endif
