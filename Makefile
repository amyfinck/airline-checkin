CC      = gcc
RM      = rm -f
RMD     = rm -f -r
PT      = -pthread

default: all

all: main

main: main.c
	$(CC) $(PT) -o ACS main.c linked_list.c queue.c

debug:
	$(CC) -g -o ACS ACS main.c linked_list.c queue.c

clean:
	$(RM) ACS
	$(RMD) ACS.dSYM
