# Suika Blog
New generation blog

## Run
install dependencies

```bash
sudo apt update
sudo apt-get install libsqlite3-dev
```

then, start the server via 
```bash
make
./bin/suika
```

## Test/Debug
```bash
make test       # build test executable
make debug      # build debug executable
```