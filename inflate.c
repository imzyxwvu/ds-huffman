#include "huffman.h"

void inflate(FILE *infile, FILE *outfile)
{
	static unsigned char block[0x80000];
	static unsigned char buffer[0x100000];
	huffman_header_t hdr;
    static huffman_t maintree[512];
	fseek(infile, 0, SEEK_SET);
	while(fread(&hdr, sizeof(hdr), 1, infile) > 0) {
        if(hdr.treesize < 1 || hdr.treesize > 512) {
            fprintf(stderr, "inflate: file corrupted, invalid treesize\n");
            return;
        }
		fread(&maintree, sizeof(huffman_t), hdr.treesize, infile);
		fread(&block, 1, hdr.codelen, infile);
		huffman_inflate(maintree, hdr.blocklen, block, buffer);
		fwrite(buffer, 1, hdr.blocklen, outfile);
	}
}

int main(int argc, char *argv[])
{
	FILE *infile, *outfile;
	if(argc < 2) {
		fprintf(stderr, "Usage: %s <infile> [outfile]\n", argv[0]);
		return 0;
	}
	infile = fopen(argv[1], "rb");
	if(infile == NULL) {
		fprintf(stderr, "Can not open input file: %s", argv[1]);
		return 0;
	}
    if(argc == 2) {
        inflate(infile, stdout);
    } else {
	    outfile = fopen(argv[2], "wb");
	    if(outfile == NULL) {
		    fprintf(stderr, "Can not open output file: %s", argv[1]);
		    return 0;
	    }
	    inflate(infile, outfile);
	    fclose(outfile);
    }
	fclose(infile);
	return 0;
}
