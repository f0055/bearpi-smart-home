import socket
import sys
import json

BOARD_IP = sys.argv[1] if len(sys.argv) > 1 else "192.168.1.54"
UDP_PORT = 8888
BROADCAST_PORT = 9999

def send_cmd(cmd):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.settimeout(3)
    msg = json.dumps(cmd)
    sock.sendto(msg.encode(), (BOARD_IP, UDP_PORT))
    print(f"Sent: {msg}")
    try:
        data, addr = sock.recvfrom(512)
        print(f"Recv: {data.decode()}")
    except socket.timeout:
        print("No reply (timeout)")
    sock.close()

def listen_broadcast():
    """监听设备广播 - 发现局域网内的设备"""
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind(("", BROADCAST_PORT))
    sock.settimeout(1)
    print(f"Listening for broadcasts on port {BROADCAST_PORT}...")
    print("Press Ctrl+C to stop\n")
    devices = {}
    try:
        while True:
            try:
                data, addr = sock.recvfrom(512)
                msg = data.decode()
                info = json.loads(msg)
                ip = info.get("ip", "unknown")
                device = info.get("device", "unknown")
                status = info.get("status", "unknown")
                locked = info.get("locked", "unknown")
                temp = info.get("temperature", "-")
                humi = info.get("humidity", "-")
                key = ip
                if key not in devices:
                    print(f"[NEW] {device} at {ip} ({status})")
                devices[key] = info
                print(f"  {device} | {ip} | locked={locked} | T={temp}C H={humi}%")
            except socket.timeout:
                pass
            except json.JSONDecodeError:
                pass
    except KeyboardInterrupt:
        print(f"\nFound {len(devices)} device(s):")
        for ip, info in devices.items():
            print(f"  {info.get('device')} at {ip}")
    sock.close()

# discover不需要board_ip，直接监听广播
if len(sys.argv) > 1 and sys.argv[1] == "discover":
    listen_broadcast()
elif len(sys.argv) > 2:
    action = sys.argv[2]
    if action == "lock":
        send_cmd({"action": "lock"})
    elif action == "unlock":
        send_cmd({"action": "unlock"})
    elif action == "on":
        send_cmd({"action": "on"})
    elif action == "off":
        send_cmd({"action": "off"})
    elif action == "status":
        send_cmd({"action": "status"})
    elif action == "ac_on":
        send_cmd({"action": "ac_on"})
    elif action == "ac_off":
        send_cmd({"action": "ac_off"})
    elif action == "broadcast_on":
        send_cmd({"broadcast": True})
    elif action == "broadcast_off":
        send_cmd({"broadcast": False})
    elif action == "scene_home":
        send_cmd({"scene": "home"})
    elif action == "scene_sleep":
        send_cmd({"scene": "sleep"})
    elif action == "scene_away":
        send_cmd({"scene": "away"})
    elif action == "auto_unlock_on":
        send_cmd({"auto_unlock": True})
    elif action == "auto_unlock_off":
        send_cmd({"auto_unlock": False})
    elif action.startswith("time:"):
        parts = action.split(":", 1)[1].split(",")
        send_cmd({"time_sync": {"hour": int(parts[0]), "minute": int(parts[1])}})
    elif action.startswith("schedule:"):
        # schedule:index,enabled,startH,startM,endH,endM
        parts = action.split(":", 1)[1].split(",")
        send_cmd({"set_schedule": {
            "index": int(parts[0]),
            "enabled": parts[1] == "1",
            "start_hour": int(parts[2]),
            "start_minute": int(parts[3]),
            "end_hour": int(parts[4]),
            "end_minute": int(parts[5])
        }})
    elif action == "midea_discover":
        send_cmd({"midea_discover": True})
    elif action.startswith("midea_on:"):
        temp = int(action.split(":", 1)[1])
        send_cmd({"midea_control": {"on": True, "temp": temp, "mode": "cool"}})
    elif action == "midea_off":
        send_cmd({"midea_control": {"on": False, "temp": 26, "mode": "cool"}})
    elif action == "discover":
        listen_broadcast()
    elif action.startswith("ac_brand:"):
        send_cmd({"ac_brand": action.split(":", 1)[1]})
    elif action.startswith("ac_mode:"):
        send_cmd({"ac_mode": action.split(":", 1)[1]})
    elif action.startswith("ac_temp:"):
        send_cmd({"ac_target_temp": int(action.split(":", 1)[1])})
    elif action.startswith("midea_token:"):
        token = action.split(":", 1)[1]
        send_cmd({"midea_token": token})
    elif action == "midea_pair":
        send_cmd({"midea_discover": True})
        send_cmd({"midea_token": "00000000000000000000000000000000"})
    else:
        send_cmd(json.loads(action))
else:
    print("Usage:")
    print("  python test_udp.py discover                  - 发现设备")
    print("  python test_udp.py <board_ip> lock")
    print("  python test_udp.py <board_ip> unlock")
    print("  python test_udp.py <board_ip> on")
    print("  python test_udp.py <board_ip> off")
    print("  python test_udp.py <board_ip> status")
    print("  python test_udp.py <board_ip> ac_on")
    print("  python test_udp.py <board_ip> ac_off")
    print("  python test_udp.py <board_ip> broadcast_on     - 开启广播")
    print("  python test_udp.py <board_ip> broadcast_off    - 关闭广播")
    print("  python test_udp.py <board_ip> scene_home       - 回家模式")
    print("  python test_udp.py <board_ip> scene_sleep      - 睡眠模式")
    print("  python test_udp.py <board_ip> scene_away       - 离家模式")
    print("  python test_udp.py <board_ip> auto_unlock_on   - 开启自动解锁")
    print("  python test_udp.py <board_ip> auto_unlock_off  - 关闭自动解锁")
    print("  python test_udp.py <board_ip> time:8,30        - 同步时间8:30")
    print("  python test_udp.py <board_ip> schedule:0,1,8,0,9,0  - 计划0:8:00-9:00开启")
    print("  python test_udp.py <board_ip> midea_discover   - 发现美的空调")
    print("  python test_udp.py <board_ip> midea_on:26      - 开美的空调26度")
    print("  python test_udp.py <board_ip> midea_off        - 关美的空调")
    print("  python test_udp.py <board_ip> ac_brand:gree")
    print("  python test_udp.py <board_ip> ac_mode:cool")
    print("  python test_udp.py <board_ip> ac_temp:25")
    print("  python test_udp.py <board_ip> midea_token:<32hex>  - 配对Token")
    print("  python test_udp.py <board_ip> '{\"brightness\":50}'")
