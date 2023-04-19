# Python client for Netlink interface

A simple python module for the kernel netlink interface.


## Installition

#### Installition via pip
`
pip install netlink-client
`

#### Installition from source
To install from source:

```
git clone https://github.com/BoazTene/netlink
cd netlink
python3 setup.py install
```


* Make sure you have gcc installed + cython

## Usage


The usage is pretty straight forward.

```
from netlink import NetLink

netlink = NetLink(31) // 31 is the magic number

netlink.send(b"Hello Netlink!")
result = netlink.recv(1024) // number of bytes

print(result.decode('utf8')) // prints result from kernel.

netlink.close() // closes the connection
```

netlink Copyright (C) 2023 Boaz Tene
