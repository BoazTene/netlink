"""
    Python client for the Netlink interface.
    Copyright (C) 2023 Boaz Tene

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
"""

from netlink import AttributePolicy, CB_Kind, CB_Type, Attribute, Message

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

    SEND = 1
    RECV = 2
    

class CustomFamily(GenericNetLink):
    """
        An example of an custom family (implemented also in a kernel module).

        structure of the family called `custom_family`:
            Atrributes:
                MSG_A - string with a min len of 0 and a max len of 100.
            Commands:
                SEND - send a message to the kernel, and the kernel will log the message in journal files.
                RECV - A message sent from the kernel, including a reply message.
    """

    FAMILY_NAME = "custom_family"
    VERSION = 1
    POLICY = [
            AttributePolicy(1, 0, 300) # policy for attribute 0. type: 1 (string), min length: 0, max length: 100
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

        if cmd == CustomFamilyCommands.RECV:
            print("[+] Received RECV command")
            
            for attribute in attributes:
                if attribute.type == CustomFamilyAttributes.MSG_A:
                    print("   * Received %d bytes from kernel" % attribute.len)
                    print("   * Data: ", attribute.data)
                  
                    
       
    def send_command(self, data: bytes):
        """
            An implmention of the test command.

            The test command sends data and receives result.
        """

        message = GenericMessage(self.get_family_id(), 0, CustomFamilyCommands.SEND, CustomFamily.VERSION)
        message.nla_put(data, 1) # Adding new attribute with a type of string
        
        print("[+] Sending a SEND command.")
        self.send(message)
        self.recv()


if __name__ == "__main__":
    family = CustomFamily()
    family.send_command(b"Just testing...")
