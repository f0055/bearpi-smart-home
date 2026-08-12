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

| 组件       | 型号                    | GPIO           |
| ---------- | ----------------------- | -------------- |
| 开发板     | BearPi-HM_Nano (Hi3861) | -              |
| PIR 传感器 | HW-870 反射式红外传感器 | GPIO_6         |
| 门磁传感器 | FC-33对射式红外传感器   | GPIO_9         |
| 舵机       | SG90 360° 连续旋转     | GPIO_13        |
| LED        | 带限流电阻 LED 模块     | GPIO_2 (PWM)   |
| 温湿度     | SHT30                   | GPIO_0/1 (I2C) |
| 按键       | F1/F2                   | GPIO_11/12     |

## GPIO 接线表

```
GPIO_0  ──── SHT30 SDA
GPIO_1  ──── SHT30 SCL
GPIO_2  ──── LED(+限流电阻)
GPIO_6  ──── HW-870传感器 OUT
GPIO_9  ──── FC-33传感器 OUT
GPIO_11 ──── F1按键(接GND)
GPIO_12 ──── F2按键(接GND)
GPIO_13 ──── SG90舵机信号线(橙)
```

## 项目结构

```
bearpi-smart-home/
├── firmware/                    # 固件源码
│   ├── BUILD.gn                 # 编译配置
│   └── src/                     # 源文件（按模块拆分）
├── app/                         # HarmonyOS App（ ArkTS）
│   └── entry/src/main/
│       ├── ets/
│       │   ├── pages/           # 页面组件
│       │   ├── components/      # 通用组件
│       │   ├── model/           # 数据模型
│       │   └── utils/           # 工具类
│       └── resources/           # 资源文件
├── tools/                       # Python 测试脚本
├── bin/                         # 编译好的固件
└── README.md
```

## 编译方法

### 固件编译

在 OpenHarmony 编译环境中执行：

```bash
hpm dist
```

编译产物位于 `out/BearPi-HM_Nano/Hi3861_wifiiot_app_allinone.bin`

### App 编译

1. 使用 DevEco Studio 打开 `app/` 目录
2. 等待 ohpm 自动安装依赖
3. Build → Build Hap(s)

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

## UDP 通信协议

App 与固件通过 UDP 协议通信，端口 8888（控制）/ 9999（广播发现）。

### App → 固件

| 功能      | JSON 指令                                                                                                                |
| --------- | ------------------------------------------------------------------------------------------------------------------------ |
| LED 开/关 | `{"action":"on"}` / `{"action":"off"}`                                                                               |
| 亮度调节  | `{"brightness": 50}`                                                                                                   |
| 门锁开/关 | `{"action":"lock"}` / `{"action":"unlock"}`                                                                          |
| 自动门禁  | `{"auto_unlock": true}` / `{"auto_unlock": false}`                                                                   |
| 空调开/关 | `{"action":"ac_on"}` / `{"action":"ac_off"}`                                                                         |
| 空调模式  | `{"ac_mode": "cool"}`                                                                                                  |
| 空调温度  | `{"ac_target_temp": 26}`                                                                                               |
| 空调品牌  | `{"ac_brand": "midea"}`                                                                                                |
| 场景切换  | `{"scene": "home"}`                                                                                                    |
| 时间同步  | `{"time_sync": {"hour": 14, "minute": 30}}`                                                                            |
| 定时器    | `{"set_timer": {"timer_id": 0, "enable": 1, "action": "on", "delay_sec": 300}}`                                        |
| 时间计划  | `{"set_schedule": {"index": 0, "enabled": true, "start_hour": 8, "start_minute": 0, "end_hour": 22, "end_minute": 0}}` |
| 状态查询  | `{"action": "status"}`                                                                                                 |
| 广播开关  | `{"broadcast": true}` / `{"broadcast": false}`                                                                       |
| 美的发现  | `{"midea_discover": true}`                                                                                             |
| 美的配对  | `{"midea_token": "00112233aabbccdd..."}`                                                                               |
| 美的控制  | `{"midea_control": {"on": true, "temp": 26, "mode": "cool"}}`                                                          |

### 固件 → App

**状态消息字段**：`status`, `brightness`, `uptime`, `locked`, `pir`, `door_open`, `auto_unlock`, `temperature`, `humidity`, `ac_on`, `ac_mode`, `ac_target_temp`, `ac_brand`, `broadcast`, `scene`, `midea_found`, `midea_ip`, `midea_paired`, `midea_device_id`, `time`, `schedule_count`

**事件消息**：`{"event":"pir_detected"}` / `{"event":"pir_left"}` / `{"event":"door_open"}`

## 测试脚本

```bash
# 全指令测试
python tools/test_udp.py

# 设备发现测试（15秒）
python tools/test_discover.py

# 监听广播
python tools/listen_broadcast.py
```

## 技术文档

### 开发环境

| 工具          | 版本要求    |
| ------------- | ----------- |
| DevEco Studio | 3.1.0+      |
| SDK           | 4.1.0(11)   |
| OpenHarmony   | 3.2 Release |
| Node.js       | 16+         |

### BearPi-HM Nano技术文档

百度网盘下载（提取码：1234）：
https://pan.baidu.com/s/1FTWszqXGOMHRciCndJO3gQ

包含内容：

- 主仓库代码
- 开发板文档资料
- 视频课程链接
- 开发者论坛

### 常见问题

**Q: App 无法连接设备？**
A: 确保手机和开发板在同一局域网，检查防火墙是否放行 UDP 8888/9999 端口。

**Q: 固件烧录失败？**
A: 检查串口驱动是否安装，尝试按住 BOOT 键后重新上电进入下载模式。

**Q: 美的空调无法控制？**
A: 确保空调处于红外遥控模式，App 已完成美的设备发现和配对。
