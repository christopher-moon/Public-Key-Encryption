CC = clang
CFLAGS = -Wall -Werror -Wextra -Wpedantic $(shell pkg-config --cflags gmp)
LFLAGS = $(shell pkg-config --libs gmp)

all: keygen encrypt decrypt

decrypt: decrypt.o ss.o numtheory.o randstate.o
	$(CC) -o $@ $^ $(LFLAGS)

encrypt: encrypt.o ss.o numtheory.o randstate.o
	$(CC) -o $@ $^ $(LFLAGS)

keygen: keygen.o ss.o numtheory.o randstate.o
	$(CC) -o $@ $^ $(LFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f keygen *.o
	rm -f encrypt *.o
	rm -f decrypt *.o

format:
	clang-format -i -style=file *.c
	clang-format -i -style=file *.h
