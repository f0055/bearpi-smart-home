import socket
import json
import sys

BOARD_IP = "192.168.1.31"
UDP_PORT = 8888

cmd = sys.argv[1] if len(sys.argv) > 1 else "unlock"
msg = json.dumps({"action": cmd})
print(f"Sending: {msg}")
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.sendto(msg.encode(), (BOARD_IP, UDP_PORT))
sock.close()
print("Done")
