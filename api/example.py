import suikaAPI
import time

ipc_session = suikaAPI.IPC()
ipc_session.init_ipc()

time.sleep(4)
ipc_session.read_ipc()