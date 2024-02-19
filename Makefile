CC=gcc

LDFLAGS=-lsqlite3
CFLAGS=-Wall -Iinclude
LIBDIR=./lib
SRCDIR=./src
UTILSDIR=./utils

ifeq ($(DEBUG), 1)
	CFLAGS+=-DDEBUG
endif

ifeq ($(TEST), 1)
	CFLAGS+=-DDEBUG -DTEST
endif

	
# executation source
SRC=suika.c $(wildcard $(LIBDIR)/*.c) $(wildcard $(SRCDIR)/*.c) $(wildcard $(UTILSDIR)/*.c)
OUT=suika

OUTDIR=bin

OUT=$(OUTDIR)/suika
all: $(OUTDIR) $(OUT)

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(OUT): $(OBJS)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

debug:
	$(MAKE) DEBUG=1

test:
	$(MAKE) TEST=1

clean:
	rm -rf $(OUTDIR) $(OBJDIR)

gzip:
	find ./theme/ \( -name '*.js' -o -name '*.css' -o -name '*.html' \) -exec gzip {} \;

gunzip:
	find ./theme/ \( -name '*.js.gz' -o -name '*.css.gz' -o -name '*.html.gz' \) -exec gunzip {} \;
