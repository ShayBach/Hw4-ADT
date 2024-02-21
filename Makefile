CC=gcc
CCLINK=$(CC)
CFLAGS=-g -Wall -std=c99
LIB=libgrades.so
RM=rm -rf 

$(LIB): grades.o
	$(CCLINK) -shared grades.o -o $(LIB) -llinked-list -L.

grades.o: grades.c grades.h linked-list.h
	$(CC) $(CFLAGS) -c -fpic grades.c

clean:
	$(RM) grades.o $(LIB) 
