# Python client for Netlink interface

A simple python module for connecting to a netlink.


## Installtion

##### Installtion via pip
`
pip install netlink
`

#### Installtion from source

`
git clone http://github.com/BoazTene/netlink
cd netlink
python3 setup.py install
`

* Make sure you have gcc installed + cython

## Usage


The usage is pretty straight forward.

Connencting to a netlink:
```
from netlink import NetLink

netlink = NetLink(31) // 31 is the magic number

netlink.send(b"Hello Netlink!")
result = netlink.recv(1024) // number of bytes

print(result.decode('utf8'))

netlink.close() // closes the connection
```


