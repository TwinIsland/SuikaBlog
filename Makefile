CC=gcc

LDFLAGS= -lsqlite3 
CFLAGS=-Wall -Iinclude

LIBDIR=./lib
LIB_SRC=$(wildcard $(LIBDIR)/*.c)
LIB_OBJ=$(LIB_SRC:.c=.o)
LIB_OUT=$(LIBDIR)/liblibrary.a

SRCDIR=./src
UTILSDIR=./utils

ifeq ($(RELEASE), 1)
	CFLAGS+=-O3
endif

ifeq ($(DEBUG), 1)
	CFLAGS+=-DDEBUG -g
endif

ifeq ($(TEST), 1)
	CFLAGS+=-DDEBUG -DTEST
endif

# Execution source
SRC=suika.c $(wildcard $(LIBDIR)/*.c) $(wildcard $(SRCDIR)/*.c) $(wildcard $(UTILSDIR)/*.c)
OUTDIR=bin
OUT=$(OUTDIR)/suika

all: $(OUTDIR) $(OUT)

$(LIB_OUT): $(LIB_OBJ)
	ar rcs $@ $(LIB_OBJ)

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(OUT): $(SRC) $(LIB_OUT)
	$(CC) -export-dynamic $(CFLAGS) $^ -o $@ $(LDFLAGS)

release:
	$(MAKE) RELEASE=1

debug:
	$(MAKE) DEBUG=1

test:
	$(MAKE) TEST=1

clean:
	rm -rf $(OUTDIR)

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