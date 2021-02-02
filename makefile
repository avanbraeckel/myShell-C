# Austin Van Braeckel - 1085929 - avanbrae@uoguelph.ca
# 2021-01-29
# CIS*3110 Assignment 1 Makefile

CC = gcc
CFLAGS = -std=gnu99 -Wpedantic -Wall -g

myShell: myShell.o
	$(CC) $(CFLAGS) -o myShell myShell.o

myShell.o: myShell.c
	$(CC) $(CFLAGS) -c myShell.c

clean:
	rm -fv *.o