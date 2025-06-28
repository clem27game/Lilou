all: main

CC = clang
override CFLAGS += -g -Wno-everything -pthread -lm

SRCS = $(shell find . -name '.ccls-cache' -type d -prune -o -type f -name '*.c' -print)
HEADERS = $(shell find . -name '.ccls-cache' -type d -prune -o -type f -name '*.h' -print)

main: $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) $(SRCS) -o "$@"

main-debug: $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) -O0 $(SRCS) -o "$@"

clean:
	rm -f main main-debug
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = main
DEBUG_TARGET = main-debug

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -O2 -o $(TARGET) main.c

$(DEBUG_TARGET): main.c
	$(CC) $(CFLAGS) -g -DDEBUG -o $(DEBUG_TARGET) main.c

clean:
	rm -f $(TARGET) $(DEBUG_TARGET) *.o

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/lilou

.PHONY: all clean install
