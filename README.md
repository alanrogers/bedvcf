# bedvcf
Filter a vcf file, selecting intervals specified in a bed file.

usage: bedvcf in.bed < in.vcf > out.vcf

This program reads a .vcf file on standard input and a .bed file
specified in a command-line argument. The .bed file specifies a set of
intervals, and the .vcf file describes variable sites in a set of DNA
sequences. The program bedvcf filters the vcf file, printing only
those intervals listed in the .bed file.

On input, the chromosomes of both files must be sorted in dictionary
order, and the nucleotide position must be sorted in numeric
order. For example, the bed file can be sorted by "sort -k1 -k2n".

The same job can be done, using bcftools, like this:

    bcftools view --output-file out.vcf --output-type v --regions-file in.bed \
      in.vcf

NOTE TO SELF: I need to check that this works with uncompressed .bed
file. I've only done this with a compressed file.

But bedvcf does this job about 500 times faster. You don't realize
this entire speed advantage, because disk io dominates the execuation
time. Working with gzipped files, (piped through zcat) I get about a
100-fold increase in speed. It is slightly slower to work with
uncompressed files, because you have to read more data.

Details about .bed format and .vcf format are available here:

    https://www.genomatix.de/online_help/help_regionminer/bedformat_help.html
    https://samtools.github.io/hts-specs/VCFv4.2.pdf

# Installation

To put the executable in $HOME/bin, type "make" and then "make install".
