#include "huffman.h"

void deflate(FILE *infile, FILE *outfile)
{
	static unsigned char block[BLOCK_SIZE];
	static unsigned char buffer[BLOCK_SIZE * 2];
	huffman_header_t hdr;
	static huffman_t maintree[512];
	size_t nread;
	while((nread = fread(block, 1, BLOCK_SIZE, infile)) > 0) {
		hdr.blocklen = nread;
		hdr.treesize = huffman_initialize_tree(block, maintree);
		hdr.codelen = huffman_deflate(maintree, block, nread, buffer);
		fwrite(&hdr, sizeof(hdr), 1, outfile);
		fwrite(maintree, sizeof(huffman_t), hdr.treesize, outfile);
		fwrite(buffer, 1, hdr.codelen, outfile);
	}
}

int main(int argc, char *argv[])
{
	FILE *infile, *outfile;
	if(argc != 3) {
		fprintf(stderr, "Usage: %s <infile> <outfile>\n", argv[0]);
		return 0;
	}
	infile = fopen(argv[1], "rb");
	if(infile == NULL) {
		fprintf(stderr, "Can not open input file: %s", argv[1]);
		return 0;
	}
	outfile = fopen(argv[2], "wb");
	if(outfile == NULL) {
		fprintf(stderr, "Can not open output file: %s", argv[1]);
		return 0;
	}
	deflate(infile, outfile);
	fclose(infile);
	fclose(outfile);
	return 0;
}
