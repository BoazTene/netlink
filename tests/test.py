from netlink import NetLink

netlink = NetLink(31)
netlink.send(b"Hello World!")
result = netlink.recv(1024)

print(result.decode('utf8'))
