CC = gcc
CFLAGS = -fPIC -Wall -Wextra -I../include
LDFLAGS = -shared
PLUGIN_DIR = ./src
SRC_DIR = ../src
INCLUDE_DIR = ../include
PLUGINS = $(wildcard $(PLUGIN_DIR)/*.c)
TARGETS = $(PLUGINS:$(PLUGIN_DIR)/%.c=%.suika)

.PHONY: all clean

all: $(TARGETS)

%.suika: $(PLUGIN_DIR)/%.c 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)
