# 快速开始指南（克隆项目后）

## 项目克隆

```bash
git clone https://github.com/f0055/bearpi-smart-home.git
cd bearpi-smart-home
```

---

## 一、固件端（BearPi-HM_Nano 开发板）

### 前置条件

- VMware Workstation + OpenHarmony 编译环境（已配置好 `hpm` 命令）
- MobaXterm（SSH连接VMware）
- RaiDrive（映射虚拟机目录到Windows）
- HiBurn（Windows烧录工具）
- USB线连接开发板

### 步骤1：修改WiFi配置

编辑 `firmware/src/gpio_config.h`，第12-13行：

```c
#define WIFI_SSID       "你的WiFi名称"
#define WIFI_PASSWORD   "你的WiFi密码"
```

### 步骤2：编译

通过MobaXterm SSH连接虚拟机：

```bash
cd ~/code/code1
hpm dist
```

编译成功后产物在：
```
out/BearPi-HM_Nano/Hi3861_wifiiot_app_allinone.bin
```

### 步骤3：烧录

1. 打开 HiBurn
2. 选择 COM 口（设备管理器查看），波特率 **921600**
3. 点击 **Select File**，选择 `Hi3861_wifiiot_app_allinone.bin`
4. 勾选 **Auto burning**
5. 按下开发板 **RST** 按键
6. 等待烧录完成，板子自动重启

### 步骤4：验证

串口工具（115200波特率）查看日志，应看到：

```
========================================
  Smart Light Controller v2.0 (UDP)
  BearPi-HM_Nano (Hi3861)
========================================

[LED] Initialized
[Servo] Initialized (GPIO_13)
[PIR] Initialized
[DoorSensor] Initialized
[Button] Initialized (F1=GPIO_11, F2=GPIO_12)
[I2C] Initialized
[SHT30] addr=0x44
[WiFi] Connected to xxx
[WiFi] Board IP: 192.168.x.x
[UDP] Listening on port 8888
[Broadcast] Board IP: 192.168.x.x, discovery broadcasting every 5s
[System] Smart Light is ready!
```

### 步骤5：用Python脚本测试（可选）

电脑与开发板在同一局域网：

```bash
# 测试所有指令
python tools/test_udp.py

# 测试设备发现
python tools/test_discover.py

# 监听广播
python tools/listen_broadcast.py
```

---

## 二、App端（HarmonyOS 手机）

### 前置条件

- DevEco Studio 3.1+
- 华为手机（HarmonyOS 2.0+）开启开发者模式 + USB调试
- 手机连接到与开发板相同的WiFi网络

### 步骤1：打开项目

1. 打开 DevEco Studio
2. **File → Open** → 选择 `bearpi-smart-home/app/` 目录
3. 等待工程同步完成

### 步骤2：配置设备

1. 手机通过USB连接电脑
2. DevEco Studio 自动识别设备
3. 顶部工具栏选择你的手机设备

### 步骤3：运行

1. 点击 **Run** (绿色三角按钮)
2. App自动安装到手机并启动
3. App启动后自动搜索局域网内的开发板

### 步骤4：使用

1. **设备发现页**：等待自动发现 "BearPi-HM_Nano"，点击连接
2. **主控页**：查看设备状态，进入各功能子页面
3. **照明**：开关灯、调节亮度
4. **门禁**：手动锁门/解锁、开启自动门禁
5. **空调**：选择品牌、模式、温度
6. **场景**：一键切换回家/睡眠/离家

---

## 三、联调测试

| 测试项 | 操作 | 预期结果 |
|--------|------|---------|
| LED开关 | App点"开灯" | 板子LED亮，串口打印 `[LED] Turn ON` |
| LED调光 | App拖动亮度条 | LED亮度跟随变化 |
| 门锁 | App点"锁门" | 舵机转动，串口打印 `[Door] Locked` |
| PIR检测 | 人经过PIR传感器 | App收到 `pir_detected` 事件 |
| 温湿度 | 查看App温湿度页 | 每2秒更新一次 |
| 空调 | App点"开机" | 串口打印 `[AC] Turned ON` |
| 场景 | App点"回家模式" | LED亮+解锁+空调开 |
| 设备发现 | 板子上电 | App自动发现设备 |

---

## 四、常见问题

**Q：编译报错 `-Werror`？**
A：所有警告都是错误，检查代码中未使用的变量、类型不匹配等问题。

**Q：板子连不上WiFi？**
A：确认 `gpio_config.h` 中的SSID和密码正确，确认路由器在附近。

**Q：App搜不到设备？**
A：确认手机和板子在同一局域网；确认板子串口日志显示 `Board IP: 192.168.x.x`。

**Q：烧录失败？**
A：降低波特率到115200；确认USB线是数据线非充电线；按住RST再点HiBurn的Download。

**Q：SHT30读数全是0？**
A：检查GPIO_0/1接线是否松动；USB热插拔后可能需要重新上电。
