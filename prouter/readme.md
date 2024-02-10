## Python Integration
Suika blog system support router integration, which means that you can create routers for blog using your favorite python framework.

### APIs
**IPC Communication**: communicate with C routers via mmap

```python
# read the content sent by C
test = IPC()
test.init_ipc()
time.sleep(4)
test.read_ipc()
```