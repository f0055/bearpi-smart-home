# 烧录说明

## 文件说明

- `Hi3861_wifiiot_app_allinone.bin` - 完整固件（包含 boot + app），推荐使用
- `Hi3861_wifiiot_app_ota.bin` - OTA 升级包
- `Hi3861_wifiiot_app_burn.bin` - 烧录专用包

## 烧录步骤

1. 安装 HiBurn 工具（Windows）
2. 用 USB 线连接 BearPi-HM_Nano 开发板
3. 打开设备管理器，查看 COM 口编号
4. 打开 HiBurn，选择对应 COM 口
5. 波特率设置为 **921600**
6. 点击 **Select File**，选择 `Hi3861_wifiiot_app_allinone.bin`
7. 勾选 **Auto burning**
8. 按下开发板上的 **RST** 按键，开始烧录
9. 等待进度条走完，烧录成功

## 注意事项

- 烧录前确保板子已通过 USB 连接电脑
- 如果烧录失败，尝试降低波特率到 115200
- 烧录完成后板子会自动重启
