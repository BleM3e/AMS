LIBMASTIK = -I./Mastik/include -L./Mastik/lib -l:libmastik.a
LIBMEM = -lrt

all: send recv

.PHONY : send recv

send:
	gcc -g -O2 send.c -o send $(LIBMASTIK) $(LIBMEM)

recv:
	gcc -g -O2 recv.c -o recv $(LIBMASTIK) $(LIBMEM)

clean:
	rm send recv
