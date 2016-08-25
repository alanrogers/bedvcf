#include <stdio.h>
#include <assert.h>
#include <string.h>

// chrom - The name of the chromosome (e.g. chr3, chrY, chr2_random)
// or scaffold (e.g. scaffold10671).
//
// chromStart - The starting position of the feature in the chromosome
// or scaffold. The first base in a chromosome is numbered 0.
//
// chromEnd - The ending position of the feature in the chromosome or
// scaffold. The chromEnd base is not included in the display of the
// feature. For example, the first 100 bases of a chromosome are
// defined as chromStart=0, chromEnd=100, and span the bases numbered
// 0-99.

#define CHRNAMESIZE 30

typedef struct Interval Interval;

struct Interval {
	char chr[CHRNAMESIZE];
	long unsigned start; // 0-based index of start of interval
	long undigned end;   // 1st position beyond end of interval
};

int Interval_read(Interval *ivl, FILE *fp) {
	char buff[300], *chr, *startToken, *endToken, *ptr;

	if(NULL == fgets(buff, sizeof buff, fp))
		return EOF;
	if(NULL == strchr(buff, '\n')) {
		fprintf(stderr,"%s:%s:%d: buffer overflow.\n",
				__FILE__,__func__,__LINE__);
		exit(EXIT_FAILURE);
	}

	chr = strtok_r(buff, "\t", &ptr);
	startToken = strtok_r(buff, "\t", &ptr);
	endToken = strtok_r(buff, "\t", &ptr);

	if(NULL == endToken)
}

int main(int argc, char **argv) {

	
	return 0;
}

