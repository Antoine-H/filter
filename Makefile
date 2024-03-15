CC=gcc
CFLAGS=-O3 -g -lm # -Wall -Wextra -Wpedantic -Wconversion -pendantic
EXEC=louvain

all: $(EXEC)

louvain: partition.o louvain.o
	$(CC) -o louvain partition.o louvain.o $(CFLAGS)

clean:
	rm -f *.o
	rm -f louvain
	rm -f *dat

%.o: %.c %.h
	$(CC) -o $@ -c $< $(CFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)
