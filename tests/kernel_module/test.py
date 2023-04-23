import socket
import struct

NETLINK_USER = 31

sock = socket.socket(socket.AF_NETLINK, socket.SOCK_RAW, NETLINK_USER)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_SNDBUF, 65536)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 65536)
sock.bind((0, 0))

msg = b"hey"
nlmsg_len = len(msg)
nlmsg_type = 0
nlmsg_flags = 0
nlmsg_seq = 0
nlmsg_pid = 0 
msg_data = struct.pack("=LHHLL", nlmsg_len, nlmsg_type, nlmsg_flags, nlmsg_seq, nlmsg_pid) + msg
sock.sendto(msg_data, (0, 0))
print(sock.recv(1024))

