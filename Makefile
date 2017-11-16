all: inflate deflate

deflate: deflate.o huffman.o
	$(CC) -o $@ deflate.o huffman.o

inflate: inflate.o huffman.o
	$(CC) -o $@ inflate.o huffman.o

%.o: %.c
	$(CC) -O2 -c -o $@ $<

clean:
	$(RM) *.o ./inflate ./deflate
