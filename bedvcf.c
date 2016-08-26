#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
// On input, the bed file must be sorted with field 1 (chromosome
// label) in dictionary order and field 2 (nucleotide position) in
// numeric order.
//
// From documentation for bed format:
//
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
//
// From documentation for vcf format.
//
// POS - position:  The reference position, with the 1st base having
// position 1.  Positions are sorted numerically, in increasing order,
// within each reference sequence CHROM. It is permitted to have
// multiple records with the same POS. Telomeres are indicated by
// using positions 0 or N+1, where N is the length of the
// corresponding chromosome or contig.  (Integer, Required)
//
// In this program, positions are base-0, so I subtract 1 from the
// vcf-file position variable.

#define CHRNAMESIZE 30

typedef struct Interval Interval;

struct Interval {
    char        chr[CHRNAMESIZE];
    long unsigned start;        // 0-based index of start of interval
    long unsigned end;          // 1st position beyond end of interval
};

int         Interval_read(Interval * self, FILE * fp);
void        Interval_print(const Interval * self, FILE * fp);
int         getChrPos(int k, char chr[k], long unsigned *pos,
                      const char *buff);

int Interval_read(Interval * self, FILE * fp) {
    char        buff[300], *chr, *startToken, *endToken, *ptr;
    int         status;

    do{
        if(NULL == fgets(buff, sizeof buff, fp))
            return EOF;
    }while(buff[0] == '\n' || buff[0] == '\0');

    if(NULL == strchr(buff, '\n') && !feof(fp)) {
        fprintf(stderr, "%s:%s:%d: buffer overflow.\n",
                __FILE__, __func__, __LINE__);
        exit(EXIT_FAILURE);
    }

    chr = strtok_r(buff, "\t", &ptr);
    startToken = strtok_r(NULL, "\t", &ptr);
    endToken = strtok_r(NULL, "\t", &ptr);

    if(NULL == endToken) {
        fprintf(stderr, "%s:%s:%d: Bad line in bed file.\n",
                __FILE__, __func__, __LINE__);
        exit(EXIT_FAILURE);
    }

    status = snprintf(self->chr, sizeof self->chr, "%s", chr);
    if(status >= sizeof self->chr) {
        fprintf(stderr, "%s:%s:%d: chromosome name too large."
                " Max=%d; name=\"%s\"\n",
                __FILE__, __func__, __LINE__, CHRNAMESIZE, chr);
        exit(EXIT_FAILURE);
    }
    // In bed files, the first nucleotide is position 0. I'm using the
    // same convention within this program, so no adjustment is needed
    // here. 
    self->start = strtoul(startToken, NULL, 10);
    self->end = strtoul(endToken, NULL, 10);

    return 0;
}

void Interval_print(const Interval * self, FILE * fp) {
    fprintf(fp, "Ivl: %5s [%9lu, %9lu)\n", self->chr, self->start, self->end);
}

// Get chromosome label and nucleotide position from buff, which
// contains a line from a vcf file. Return -1 on failure, 0 on success. 
int getChrPos(int k, char chr[k], long unsigned *pos, const char *buff) {
    // I'm tokenizing by hand, because strsep or strtok_r would
    // modify buff, and I don't want to do that. sscanf would
    // treat tabs as ordinary whitespace, but I need to treat each
    // tab as a field separator.

    // tab1 and tab2 point to the first two tabs
    char       *tab1, *tab2 = NULL;
    char        postok[CHRNAMESIZE];
    tab1 = strchr(buff, '\t');
    if(tab1 != NULL)
        tab2 = strchr(tab1 + 1, '\t');
    if(tab2 == NULL) {
        fprintf(stderr, "%s:%d: Bad line in vcf file:\n %s\n",
                __FILE__, __LINE__, buff);
        exit(EXIT_FAILURE);
    }
    // copy first token into chr
    int         n = tab1 - buff;
    if(n >= k) {
        fprintf(stderr, "%s:%s:%d: buffer overflow.\n",
                __FILE__, __func__, __LINE__);
        exit(EXIT_FAILURE);
    }
    strncpy(chr, buff, n);
    chr[n] = '\0';

    // copy 2nd token into postok
    tab1 += 1;
    n = tab2 - tab1;
    if(n >= CHRNAMESIZE) {
        fprintf(stderr, "%s:%s:%d: buffer overflow.\n",
                __FILE__, __func__, __LINE__);
        exit(EXIT_FAILURE);
    }
    strncpy(postok, tab1, n);
    postok[n] = '\0';

    *pos = strtoul(postok, NULL, 10);
    if(pos == 0)                // in telomere
        return -1;
    --*pos;                     // make consistent with bed
    return 0;
}

const char *usage = "usage: bedvcf file.bed < file.vcf";
int main(int argc, char **argv) {

    if(argc != 2) {
        fprintf(stderr, "%s\n", usage);
        exit(EXIT_FAILURE);
    }

    FILE       *bed = fopen(argv[1], "r");
    if(bed == NULL) {
        fprintf(stderr, "Can't open bed file \"%s\".\n", argv[1]);
        fprintf(stderr, "%s\n", usage);
        exit(EXIT_FAILURE);
    }

    Interval    ivl;

    if(EOF == Interval_read(&ivl, bed)) {
        fprintf(stderr, "Can't read initial interval from bed file.\n");
        exit(EXIT_FAILURE);
    }
    //    Interval_print(&ivl, stdout);

    char        buff[2048];
    char        chr[CHRNAMESIZE];
    unsigned long pos;
    int         chrdiff = -1;
    while(1) {
        if(NULL == fgets(buff, sizeof buff, stdin))
            break;
        if(NULL == strchr(buff, '\n') && !feof(stdin)) {
            fprintf(stderr, "%s:%s:%d: buffer overflow.\n",
                    __FILE__, __func__, __LINE__);
            exit(EXIT_FAILURE);
        }
        // Header lines pass through unfiltered
        if(buff[0] == '#') {
            if(EOF == fputs(buff, stdout)) {
                fprintf(stderr, "%s:%d: Error on output\n", __FILE__,
                        __LINE__);
                exit(EXIT_FAILURE);
            }
            continue;
        }

        // Parse buff to get chr and pos. Non-zero return
        // means we're in a telomere, so read another line.
        if(getChrPos(CHRNAMESIZE, chr, &pos, buff))
            continue;

        //        fputs(buff, stdout);
        //        printf("postok=\"%s\" pos=%lu\n", postok, pos);

        chrdiff = strcmp(chr, ivl.chr);
        if(chrdiff<0 || (chrdiff==0 && pos < ivl.start))
            // We're not yet at the start of interval, so read vcf.
            continue;

        assert(chrdiff >= 0);
        while(chrdiff>0 || (chrdiff==0 && pos >= ivl.end)) {
            // We're beyond end of interval so read bed file.
            Interval    prev = ivl;
            if(EOF == Interval_read(&ivl, bed))
                goto done;

            //            Interval_print(&ivl, stdout);
            chrdiff = strcmp(prev.chr, ivl.chr);
            if(chrdiff > 0) {
                fprintf(stderr,
                        "%s:%d: chromosomes in bed file aren't sorted.\n",
                        __FILE__, __LINE__);
                fprintf(stderr, "       %s precedes %s\n", prev.chr, ivl.chr);
                exit(EXIT_FAILURE);
            } else if(chrdiff == 0 && prev.start > ivl.start) {
                fprintf(stderr, "%s:%d: positions in bed file aren't sorted.\n",
                        __FILE__, __LINE__);
                fprintf(stderr, "       %lu precedes %lu\n",
                        prev.start, ivl.start);
                exit(EXIT_FAILURE);
            }
            chrdiff = strcmp(chr, ivl.chr);
        }

        // New interval doesn't include current position: read vcf.
        if(chrdiff < 0 || pos < ivl.start)
            continue;

        // In the interval: print a line of output.
        assert(chrdiff==0 && pos >= ivl.start && pos < ivl.end);
        if(EOF == fputs(buff, stdout)) {
            fprintf(stderr, "%s:%d: Error on output\n", __FILE__, __LINE__);
            exit(EXIT_FAILURE);
        }
    }
  done:
    return 0;
}
