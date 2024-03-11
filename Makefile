CC=gcc

LDFLAGS=-lsqlite3
CFLAGS=-Wall -Iinclude -export-dynamic
LIBDIR=./lib
SRCDIR=./src
UTILSDIR=./utils

ifeq ($(DEBUG), 1)
	CFLAGS+=-DDEBUG
endif

ifeq ($(TEST), 1)
	CFLAGS+=-DDEBUG -DTEST
endif

# Execution source
SRC=suika.c $(wildcard $(LIBDIR)/*.c) $(wildcard $(SRCDIR)/*.c) $(wildcard $(UTILSDIR)/*.c)
OUTDIR=bin
OUT=$(OUTDIR)/suika

all: $(OUTDIR) $(OUT)

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

debug:
	$(MAKE) DEBUG=1

test:
	$(MAKE) TEST=1

clean:
	rm -rf $(OUTDIR)

gzip:
	find ./theme/ \( -name '*.js' -o -name '*.css' -o -name '*.html' \) -exec gzip {} \;

gunzip:
	find ./theme/ \( -name '*.js.gz' -o -name '*.css.gz' -o -name '*.html.gz' \) -exec gunzip {} \;

run:
	make clean
	make all
	./$(OUT)

run_dev:
	make clean
	make debug
	./$(OUT)

run_release:
	make clean
	make gzip
	make all
	./$(OUT)

run_test:
	make clean
	make test
	./$(OUT)
	
before_push:
	make gunzip