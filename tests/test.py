from netlink import NetLink, Message
from guppy import hpy
from time import sleep
import os


if __name__ == "__main__":
    try:
        netlink = NetLink("FAMILY1")
    except Exception:
        print("Bug happened")
        exit(0)
    print("family_id", netlink.get_family_id())
    message = Message(netlink.get_family_id(), 0, 0, 1, 1)
    message.nla_put(bytes(b"hello world"), 1)
    netlink.send(message)
    netlink.close()
