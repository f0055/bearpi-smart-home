#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
监听板子的设备发现广播（UDP 9999）。
用法：
  1. 电脑连上和板子同一个 WiFi（CMCC-9878）
  2. 运行：python listen_broadcast.py
  3. 若每5秒收到一条 BearPi 的 JSON，说明板子广播正常、网络通
"""
import socket

PORT = 9999

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
try:
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
except OSError:
    pass
sock.bind(("", PORT))

print(f"[Listen] 正在监听 UDP {PORT} 端口，等待板子广播...")
print("[Listen] （Ctrl+C 退出）\n")

count = 0
while True:
    try:
        data, addr = sock.recvfrom(2048)
        count += 1
        print(f"[{count}] 收到来自 {addr[0]}:{addr[1]} 的广播：")
        print(f"    {data.decode('utf-8', errors='replace')}\n")
    except KeyboardInterrupt:
        print("\n[Listen] 退出")
        break
    except Exception as e:
        print(f"[Listen] 错误: {e}")
