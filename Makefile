CC=gcc

LDFLAGS= -Lsqlite3 -lsqlite3 -Wl,-rpath,sqlite3
CFLAGS=-Wall -Iinclude

LIBDIR=./lib
LIB_SRC=$(wildcard $(LIBDIR)/*.c)
LIB_OBJ=$(LIB_SRC:.c=.o)
LIB_OUT=$(LIBDIR)/liblibrary.a

# dependencies library
SQLITE3_SHARED = sqlite3/libsqlite3.so

SRCDIR=./src

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
SRC=suika.c $(wildcard $(LIBDIR)/*.c) $(wildcard $(SRCDIR)/*.c)
OUTDIR=bin
OUT=$(OUTDIR)/suika

all: $(OUTDIR) $(OUT) $(SQLITE3_SHARED)

$(LIB_OUT): $(LIB_OBJ)
	ar rcs $@ $(LIB_OBJ)

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(OUT): $(SRC) $(LIB_OUT)
	$(CC) -export-dynamic $(CFLAGS) $^ -o $@ $(LDFLAGS)


$(SQLITE3_SHARED):
	cd sqlite3 && \
	gcc -shared -fPIC -O2 -o libsqlite3.so sqlite3.c \
	-DSQLITE_THREADSAFE=0 \
	-DSQLITE_DEFAULT_MEMSTATUS=0 \
	-DSQLITE_DEFAULT_WAL_SYNCHRONOUS=1 \
	-DSQLITE_LIKE_DOESNT_MATCH_BLOBS \
	-DSQLITE_MAX_EXPR_DEPTH=0 \
	-DSQLITE_OMIT_DECLTYPE \
	-DSQLITE_OMIT_DEPRECATED \
	-DSQLITE_OMIT_SHARED_CACHE

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