# Suika Blog
Ultimate blog, born for speed ⚡

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

## Gzip
gzip is off by default, and you need to enable it manually by running:
```bash
make gzip       # turn on gzip feature
make gunzip     # turn off gzip feature
```

## Test/Debug
**Backend:**
```bash
make test       # build test executable
make debug      # build debug executable
```
**Frontend:**
> need npm to run frontend test server

```bash
cd theme/test
make test_server
```

## Frontend
The default frontend theme, [Suika Ultimate Theme](./theme/readme.md), is optimized for ultimate performance, boasting a total dependency of less than 3kb. It primarily utilizes vanilla JavaScript and CSS for streamlined efficiency.


## Thanks
+ [cesanta/mongoose](https://github.com/cesanta/mongoose)
+ [nothings/stb](https://github.com/nothings/stb)
+ [benhoyt/inih](https://github.com/benhoyt/inih)
+ [SQLite3](https://www.sqlite.)
+ [B-Con/crypto-algorithms](https://github.com/B-Con/crypto-algorithms/)
+ [franciscop/picnic](https://github.com/franciscop/picnic)
