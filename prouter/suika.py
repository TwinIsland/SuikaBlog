import mmap
import os

from inilib import SimpleIniParser

CONFIG_DIR = "../config.ini"

class IPCStatus:
    IPC_INCOMPLETE = b'0'
    IPC_SENT = b'1'
    IPC_READ = b'2'

class IPC:
    ipc_mmap = None
    IPC_FILE_PATH = None
    IPC_SIZE = None

    def __init__(self):
        parser = SimpleIniParser()
        parser.parse(CONFIG_DIR)

        parser.get('ipc', 'ipc_path')

        self.IPC_FILE_PATH = parser.get('ipc', 'ipc_path')
        self.IPC_SIZE = int(parser.get('ipc', 'ipc_size'))

    def init_ipc(self):
        with open(self.IPC_FILE_PATH, 'wb') as f:
            f.truncate(self.IPC_SIZE)
        fd = os.open(self.IPC_FILE_PATH, os.O_RDWR)
        self.ipc_mmap = mmap.mmap(fd, self.IPC_SIZE, access=mmap.ACCESS_WRITE)
        os.close(fd)

    def send_ipc(self, message):
        if self.ipc_mmap is not None:
            if isinstance(message, str):
                message = message.encode('utf-8')
            message_length = min(len(message), self.IPC_SIZE - 2)
            self.ipc_mmap[0:1] = IPCStatus.IPC_INCOMPLETE
            self.ipc_mmap[1:1 + message_length] = message[:message_length]
            self.ipc_mmap[0:1] = IPCStatus.IPC_SENT
        else:
            print("IPC not initialized or mmap failed.")

    def read_ipc(self):
        if self.ipc_mmap is not None:
            if self.ipc_mmap[0:1] == IPCStatus.IPC_SENT:
                print(self.ipc_mmap[1:].rstrip(b'\x00').decode('utf-8'))
                self.ipc_mmap[0:1] = IPCStatus.IPC_READ
            else:
                print("No new message or message already read.")
        else:
            print("IPC not initialized or mmap failed.")

    def reset_ipc(self):
        if self.ipc_mmap is not None:
            self.ipc_mmap.seek(0)
            self.ipc_mmap.write(b'\x00' * self.IPC_SIZE)
        else:
            print("IPC not initialized or mmap failed.")

    def cleanup_ipc(self):
        if self.ipc_mmap is not None:
            self.ipc_mmap.close()
            self.ipc_mmap = None
        try:
            os.remove(self.IPC_FILE_PATH)
        except OSError as e:
            print(f"Error removing IPC file: {e}")


# import time
# test = IPC()
# test.init_ipc()

# time.sleep(4)
# test.read_ipc()
