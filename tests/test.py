from netlink import NetLink
from time import sleep

netlink = NetLink(b'echo')
netlink.do(0,0,b"hello world");
