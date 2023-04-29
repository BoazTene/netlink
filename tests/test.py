from netlink import NetLink
from guppy import hpy
from time import sleep
import os


if __name__ == "__main__":
    hp = hpy()
    heap_before_init = hp.heap()
    try:
        netlink = NetLink("FAMILY1")
    except Exception:
        print("Bug happened")
        print("heap before initialization:", heap_before_init)
        print("heap now:", hp.heap())
        exit(0)

    netlink.send(0,0,b"hello world")
    netlink.close()
