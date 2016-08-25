#include <stdio.h>
#include <assert.h>

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


