CC      = gcc
RM      = rm -f
RMD     = rm -f -r

default: all

all: main

main: main.c
	$(CC) -o ACS main.c

debug:
	$(CC) -g -o ACS PMan main.c

clean:
	$(RM) ACS
	$(RMD) ACS.dSYM