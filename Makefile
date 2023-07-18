CC = clang
CFLAGS = -Wall -Wpedantic -Werror -Wextra $(shell pkg-config --cflags gmp)
LFLAGS = $(shell pkg-config --libs gmp)

all: encrypt decrypt keygen


encrypt : numtheory.o randstate.o rsa.o encrypt.o
	$(CC) -o encrypt numtheory.o randstate.o rsa.o encrypt.o $(LFLAGS)

decrypt : numtheory.o randstate.o rsa.o decrypt.o
	$(CC) -o decrypt numtheory.o randstate.o rsa.o decrypt.o $(LFLAGS)

keygen : numtheory.o randstate.o rsa.o keygen.o
	$(CC) -o keygen numtheory.o randstate.o rsa.o keygen.o $(LFLAGS)


decrypt.o: decrypt.c
	$(CC) $(CFLAGS) -c decrypt.c

encrypt.o: encrypt.c
	$(CC) $(CFLAGS) -c encrypt.c

keygen.o: keygen.c
	$(CC) $(CFLAGS) -c keygen.c

numtheory.o: numtheory.c
	$(CC) $(CFLAGS) -c numtheory.c

randstate.o: randstate.c
	$(CC) $(CFLAGS) -c randstate.c

rsa.o: rsa.c
	$(CC) $(CFLAGS) -c rsa.c

clean:
	rm -f decrypt encrypt keygen numtheory randstate rsa *.o

format:
	clang-format -i -style=file *.[c,h]
