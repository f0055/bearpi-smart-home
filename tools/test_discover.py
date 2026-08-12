#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
分布式能力演示脚本 —— 设备自动发现（UDP广播）。
用法（在电脑PowerShell，电脑需连和板子同一个WiFi）：
    python test_discover.py

运行后会监听局域网UDP广播，每收到一条板子的广播就打印出来。
演示"板子上电自动广播，App/工具无需手动配置IP即可发现设备"。

按 Ctrl+C 退出。
"""
import socket
import json
import time

BROADCAST_PORT = 9999
LISTEN_SECONDS = 15  # 监听时长（秒），到时自动退出，方便录视频

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sock.bind(("", BROADCAST_PORT))
sock.settimeout(1)

print("=" * 50)
print("分布式能力演示：设备自动发现")
print(f"正在监听 UDP 广播端口 {BROADCAST_PORT} ...")
print(f"监听 {LISTEN_SECONDS} 秒后自动结束（Ctrl+C 可提前退出）")
print("=" * 50)

devices = {}
end = time.time() + LISTEN_SECONDS
n = 0
try:
    while time.time() < end:
        try:
            data, addr = sock.recvfrom(2048)
            n += 1
            info = json.loads(data.decode("utf-8", "replace"))
            ip = info.get("ip", addr[0])
            devices[ip] = info
            print(f"\n[发现设备 {n}] 来自 {ip}")
            print(f"    设备名 : {info.get('device')}")
            print(f"    状态   : {info.get('status')}")
            print(f"    门锁   : {'已锁' if info.get('locked') else '已开'}")
            print(f"    温度   : {info.get('temperature')}C")
            print(f"    湿度   : {info.get('humidity')}%")
        except socket.timeout:
            continue
        except json.JSONDecodeError:
            continue
except KeyboardInterrupt:
    print("\n(手动停止)")

print("\n" + "=" * 50)
print(f"发现设备总数: {len(devices)}")
for ip, info in devices.items():
    print(f"  -> {info.get('device')}  IP={ip}  状态={info.get('status')}")
print("=" * 50)
