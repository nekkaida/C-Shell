# Simplified Makefile for C Shell

CC := gcc
CFLAGS := -Wall -Wextra -pedantic -std=c11 -O2

shell: main.c
	$(CC) $(CFLAGS) -o shell main.c

clean:
	rm -f shell

.PHONY: clean