from netlink import NetLink, Message, CB_Kind, CB_Type, ArgumentPolicy, Attribute
import struct


class GenericMessage(Message):
    """
        Defines a generic message.

        This message is sendable and will add automatically the generic header.
    """

    def __init__(self, family_id: int, flags: int, cmd: int, version: int):
        super().__init__(family_id, 0, flags)

        self.cmd = cmd
        self.version = version
        
        self.append(self.__generate_header(), GenericNetLink.HEADER_LEN) # appends the generic header

    def __generate_header(self) -> bytes:
        """
            Generates generic netlink header.
            
            Generic Netlink header is defined as followes:
                cmd - 8 bits
                version - 8 bits
                not_used/reserved - 16 bits

            @return the header in bytes.
        """

        return struct.pack("BBH", self.cmd, self.version, 0)
    
    @staticmethod
    def from_message(message: Message):
        """
            Creates an generic message from an existing netlink message (Base Message).
            
            @param messsage src message.
            @return Generic Message.
        """

        length, family_id, flags, seq, pid = message.parse_header()
        cmd, version, _ = GenericMessage.parse_header(message.get_bytes())
        
        genl_message = GenericMessage(family_id, flags, cmd, version)

        genl_message.append(message.get_bytes()[NetLink.HEADER_LEN + GenericNetLink.HEADER_LEN:], 0) # adding payload.

        return genl_message
        
    @staticmethod
    def parse_header(bytes_data: bytes):

        return struct.unpack("BBH", bytes_data[16:20])


class GenericNetLink(NetLink):
    HEADER_LEN = 4
    PROTOCOL = 16

    def __init__(self, family_name: str, policies: list[ArgumentPolicy]):
        super().__init__(NetLink.resolve_genl_family_id(family_name), GenericNetLink.PROTOCOL, GenericNetLink.HEADER_LEN, policies)

    def parse_message(self, msg: Message) -> tuple[list[Attribute], int, int]:
        """
            Parses a message (attributes + header).
            For conviency later its receiving Message type.

            @param msg message to parse
            @return tuple of list of attributes, cmd and version.
        """

        message = GenericMessage.from_message(msg)

        attributes = self.parse_message_attributes(msg);
        cmd, version = message.cmd, message.version

        return attributes, cmd, version 
