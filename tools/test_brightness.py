import socket
import json

BOARD_IP = "192.168.1.31"
UDP_PORT = 8888

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
msg = json.dumps({"brightness": 50})
print(f"Sending: {msg}")
sock.sendto(msg.encode(), (BOARD_IP, UDP_PORT))
sock.close()
print("Done")
