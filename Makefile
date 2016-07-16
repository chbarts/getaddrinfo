CFLAGS = -O3 -g -Wall -ansi -pedantic -std=c99 -march=native -D_POSIX_C_SOURCE=200112L

CC = gcc

.SUFFIXES: .c .o

.c.o:
	$(CC) $(CFLAGS) -c $<

OBJS = getaddrinfo.o

SRCS = getaddrinfo.c

all: getaddrinfo

getaddrinfo: $(OBJS)
	$(CC) $(OBJS) -o getaddrinfo

clean:
	rm *.o getaddrinfo
