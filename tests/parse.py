from pyroute2.netlink import genlmsg
from pyroute2.netlink import nlmsg


def parse_netlink_packet(data):
    # Create a netlink message object
    msg = nlmsg(data)

    # Check if it is a generic netlink message
    if msg['type'] != genlmsg.ID:
        raise ValueError('Not a generic netlink message')

    # Parse the generic netlink message
    genl = genlmsg(msg)

    # Print the parsed message
    print(genl)


parse_netlink_packet(b"\xec\xf0\x14\xd2U\x00\x00\x80\x10\x0f\xbb(\x7f\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00@S\xae\x14\xd2U\x00\x00\x00\x00\x00\x00")
