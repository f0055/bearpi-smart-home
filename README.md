# BearPi-HM_Nano 智能家居系统

基于 OpenHarmony + BearPi-HM_Nano (Hi3861) 的国产全栈智能家居系统，通过局域网 UDP 协议实现手机 App 对门禁、照明、温湿度、空调、场景的统一控制。

## 功能特性

- **门禁控制**：SG90 360° 连续旋转舵机，自动/手动锁门，PIR 检测联动
- **LED 照明**：PWM 调光，0-100% 亮度
- **温湿度监测**：SHT30 传感器，I2C 读取
- **空调控制**：美的 mSmart 协议直连（AES-128 加密）
- **场景联动**：回家/睡眠/离家一键切换
- **设备发现**：UDP 广播，App 自动发现板子
- **本地控制**：物理按键 F1(F2) 控制 LED(门锁)
- **断网可用**：WiFi 断线自动重连，状态 RAM 备份

## 硬件清单

| 组件 | 型号 | GPIO |
|------|------|------|
| 开发板 | BearPi-HM_Nano (Hi3861) | - |
| PIR 传感器 | HW-870 反射式红外 | GPIO_6 |
| 门磁传感器 | 槽型红外传感器 | GPIO_9 |
| 舵机 | SG90 360° 连续旋转 | GPIO_13 |
| LED | 带限流电阻 LED 模块 | GPIO_2 (PWM) |
| 温湿度 | SHT30 | GPIO_0/1 (I2C) |
| 按键 | F1/F2 | GPIO_11/12 |

## GPIO 接线表

```
GPIO_0  ──── SHT30 SDA
GPIO_1  ──── SHT30 SCL
GPIO_2  ──── LED (+ 限流电阻)
GPIO_6  ──── PIR 传感器 OUT
GPIO_9  ──── 槽型传感器 OUT
GPIO_11 ──── F1 按键 (接 GND)
GPIO_12 ──── F2 按键 (接 GND)
GPIO_13 ──── SG90 舵机信号线 (橙)
```

## 编译方法

在 OpenHarmony 编译环境中执行：

```bash
hpm dist
```

编译产物位于 `out/BearPi-HM_Nano/Hi3861_wifiiot_app_allinone.bin`

## 烧录方法

1. 安装 HiBurn 工具
2. 连接开发板 USB
3. 选择 COM 口，波特率 921600
4. 选择 `Hi3861_wifiiot_app_allinone.bin`
5. 点击 Download 烧录

## WiFi 配置

**重要**：编译前请修改 `firmware/src/gpio_config.h` 第 12-13 行：

```c
#define WIFI_SSID       "YOUR_WIFI_SSID"      // 改为你的WiFi名称
#define WIFI_PASSWORD   "YOUR_WIFI_PASSWORD"   // 改为你的WiFi密码
```

## 项目结构

```
bearpi-smart-home/
├── firmware/           # 固件源码
│   ├── BUILD.gn        # 编译配置
│   └── src/            # 源文件（按模块拆分）
├── tools/              # Python 测试脚本
├── bin/                # 编译好的固件
└── README.md
```

## 测试脚本

```bash
# 全指令测试
python tools/test_udp.py

# 设备发现测试（15秒）
python tools/test_discover.py

# 监听广播
python tools/listen_broadcast.py
```

## 许可证

MIT License
