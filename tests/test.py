from netlink import NetLink, Message, CB_Kind, CB_Type, ArgumentPolicy
import struct
from guppy import hpy
from time import sleep
import os


class GenericMessage(Message):
    HEADER_LEN = 4

    def __init__(self, family_id: int, flags: int, cmd: int, version: int):
        super().__init__(family_id, 0, flags);

        self.cmd = cmd
        self.version = version
        
        self.append(self.__generate_header(), GenericMessage.HEADER_LEN)


    def __generate_header(self):
        return struct.pack("BBH", self.cmd, self.version, 0)

    @staticmethod
    def from_message(message: Message):
        length, family_id, flags, seq, pid = message.parse_header()
        cmd, version, _ = GenericMessage.parse_header(message.get_bytes())
        
        genl_message = GenericMessage(family_id, flags, cmd, version)

        genl_message.append(message.get_bytes()[16+4:], 0)

        return genl_message

    @staticmethod
    def parse_header(bytes_data: bytes):
        return struct.unpack("BBH", bytes_data[16:20])



class GenericNetLink(NetLink):
    HEADER_LEN = 4

    def __init__(self):
        super().__init__(NetLink.resolve_genl_family_id("FAMILY1"), 16, GenericNetLink.HEADER_LEN, [ArgumentPolicy(1, 1, 100)])
        self.modify_cb(CB_Type.CB_VALID, CB_Kind.CB_CUSTOM, self.recv_messages);

    def recv_messages(self, msg: Message):
        print(msg.get_bytes())
        message = GenericMessage.from_message(msg)
        attributes = self.parse_message_attributes(message);
        cmd, version = message.cmd, message.version

        print(cmd, version, attributes[0].data)
        
        return 0


if __name__ == "__main__":
    family = GenericNetLink()
    message = GenericMessage(family.get_family_id(), 0, 1, 1)
    message.nla_put(b"helloworld", 1)
    family.send(message)
    family.recv()

