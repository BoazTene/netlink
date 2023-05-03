from netlink import ArgumentPolicy, CB_Kind, CB_Type, Attribute, Message

from generic_netlink import GenericNetLink, GenericMessage


class CustomFamilyAttributes:
    """
        The family's possible attributes with ids.
    """

    MSG_A = 1
       

class CustomFamilyCommands:
    """
        The family's possible commands.
    """

    TEST = 1
    

class CustomFamily(GenericNetLink):
    """
        An example of an custom family (implemented also in a kernel module).
    """

    FAMILY_NAME = "FAMILY1"
    VERSION = 1
    POLICY = [
            ArgumentPolicy(1, 0, 100) # policy for argument 0. type: 1 (string), min length: 0, max length: 100
    ]

    def __init__(self):
        super().__init__(CustomFamily.FAMILY_NAME, CustomFamily.POLICY)

        self.__define_callbacks()

    def __define_callbacks(self):
        """
            Defines all of the callbacks the family uses.
        """

        self.modify_cb(CB_Type.CB_VALID, CB_Kind.CB_CUSTOM, self.recv_messages)

    def recv_messages(self, message: Message):
        """
            The callback for valid messages.

            @param message the new message
        """

        attributes, cmd, version = self.parse_message(message)
        
        print("Received new message: ")
        print("[+] cmd: ", cmd)
        print("[+] version: ", version)
        
        for attribute in attributes:
            print("New attribute!") 
            print("[+] type: ", attribute.type)
            print("[+] len: ", attribute.len)
            print("[+} data: ", attribute.data)

    def test_command(self, data: bytes):
        """
            An implmention of the test command.

            The test command sends data and receives result.
        """

        message = GenericMessage(self.get_family_id(), 0, CustomFamilyCommands.TEST, CustomFamily.VERSION)
        message.nla_put(data, 1) # Adding new attribute with a type of string

        self.send(message)
        self.recv()


if __name__ == "__main__":
    family = CustomFamily()
    family.test_command(b"Just testing...")
