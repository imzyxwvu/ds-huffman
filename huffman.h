#include <stdio.h>
#include <string.h>
#include <time.h>

#define BLOCK_SIZE 0x80000

typedef struct huffman_node {
	int weight;
	short lch, rch;
} huffman_t;

typedef struct huffman_control {
	int blocklen;
	int codelen;
	int treesize;
} huffman_header_t;

int huffman_initialize_tree(unsigned char *block, huffman_t *tree);
int huffman_deflate(huffman_t *tree, const unsigned char *input, size_t length, unsigned char *output);
void huffman_inflate(huffman_t *tree, int blklen, const unsigned char *inbuf, unsigned char *outbuf);
