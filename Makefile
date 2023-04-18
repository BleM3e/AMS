CC = gcc
CFLAGS = -Wall -O2 -g
LIBMASTIK = -I./Mastik/include -L./Mastik/lib -l:libmastik.a

all: send recv

.PHONY: send recv

send: send.c
	$(CC) $(CFLAGS) -o send send.c $(LIBMASTIK)

recv: recv.c
	$(CC) $(CFLAGS) -o recv recv.c -lm $(LIBMASTIK)

clean:
	rm -f send recv
