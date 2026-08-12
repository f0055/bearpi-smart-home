import socket
import struct
import time

AC_IP = "192.168.1.6"
AC_PORT = 6445

def send_packet(data):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.settimeout(5)
    sock.sendto(data, (AC_IP, AC_PORT))
    try:
        resp, addr = sock.recvfrom(1024)
        print(f"收到响应: {resp.hex()}")
        return resp
    except socket.timeout:
        print("超时")
        return None
    finally:
        sock.close()

# 发送发现包
discover = bytes([
    0x5a, 0x5a, 0x01, 0x11, 0x48, 0x00, 0x92, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
])

print("发送发现包...")
resp = send_packet(discover)
if resp:
    print(f"空调响应了! 长度: {len(resp)} 字节")
else:
    print("空调没响应")
