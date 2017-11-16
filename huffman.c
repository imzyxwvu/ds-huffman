#include "huffman.h"

typedef struct huffman_code {
	unsigned char code[24];
	int codelen;
} huffman_code_t;

static int huffman_next_free_slot(huffman_t *tree, int *size)
{
	int i;
	for(i = 0; i < 256; i++)
		if(tree[i].weight == -2)
			return i;
	return (*size)++;
}

static int huffman_step(huffman_t *tree, int *size)
{
	int min1 = -1, min2 = -1, t, i, n = 0, slot;
	for(i = 0; i < *size; i++) {
		if(tree[i].weight >= 0) {
			n++;
			if(min2 == -1 || tree[i].weight < tree[min2].weight)
				min2 = i;
			if(min1 == -1 || tree[min2].weight < tree[min1].weight)
				t = min1, min1 = min2, min2 = t;
		}
	}
	if(n < 2) return 0;
	slot = huffman_next_free_slot(tree, size);
	tree[slot].weight = tree[min1].weight + tree[min2].weight;
	tree[slot].lch = min1;
	tree[slot].rch = min2;
	tree[min1].weight = -1;
	tree[min2].weight = -1;
	return n;
}

int huffman_initialize_tree(unsigned char *block, huffman_t *tree)
{
	int i;
	int n = 256;
	for(i = 0; i < 256; i++) {
		tree[i].weight = 0;
		tree[i].lch = tree[i].rch = -1;
	}
	for(i = 0; i < BLOCK_SIZE; i++)
		tree[block[i]].weight++;
	for(i = 0; i < 256; i++)
		if(tree[i].weight == 0)
			tree[i].weight = -2;
	while(huffman_step(tree, &n));
	return n;
}

int huffman_find_root(huffman_t *tree)
{
	int i;
	for(i = 0; ; i++)
		if(tree[i].weight >= 0)
			return i;
}

void huffman_compute_code(huffman_t *tree, int node, huffman_code_t *codes, int codelen)
{
	static huffman_code_t code;
	if(node < 0) return;
	if(tree[node].lch == -1 && tree[node].rch == -1 && node < 256) {
		code.codelen = codelen;
		memcpy(&codes[node], &code, sizeof(code));
		return;
	}
	code.code[codelen] = 0;
	huffman_compute_code(tree, tree[node].lch, codes, codelen + 1);
	code.code[codelen] = 1;
	huffman_compute_code(tree, tree[node].rch, codes, codelen + 1);
}

int huffman_deflate(huffman_t *tree, const unsigned char *input, size_t length, unsigned char *output)
{
	unsigned char ch = 0;
	int i, j, k = 0, root, outpos = 0;
	static huffman_code_t code[256];
	root = huffman_find_root(tree);
	huffman_compute_code(tree, root, code, 0);
	for(i = 0; i < length; i++) {
		for(j = 0; j < code[input[i]].codelen; j++) {
			ch = ch | ((code[input[i]].code[j] & 1) << k);
			if(++k == 8) {
				output[outpos++] = ch;
				k = 0;
				ch = 0;
			}
		}
	}
	if(k > 0)
		output[outpos++] = ch;
	return outpos;
}

void huffman_inflate(huffman_t *tree, int blklen, const unsigned char *inbuf, unsigned char *outbuf)
{
	int root = huffman_find_root(tree);
	int bitno = 0;
	int m = root;
	while(blklen > 0) {
		char b = (*inbuf >> bitno++) & 1;
		m = b ? tree[m].rch : tree[m].lch;
		if(tree[m].lch == -1 && tree[m].rch == -1) {
			*(outbuf++) = m;
			m = root;
		}
		if(bitno == 8) {
			bitno = 0;
			inbuf++;
			blklen--;
		}
	}
}

