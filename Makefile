destination := $(HOME)/bin
global_destination := /usr/local/bin
#opt := -DNDEBUG -O3  -finline-functions  # For full optimization
#opt := -O3  -finline-functions  # Optimization + debugging
opt :=  -O0 -fno-inline-functions -rdynamic -DDEBUG     # For debugging
#prof := -pg -rdynamic                    # For profiling
prof :=
incl := -I/usr/local/include -I/opt/local/include

targets := bedvcf

CC := gcc

# Flags to determine the warning messages issued by the compiler
warn := \
 -Wall \
 -Wcast-align \
 -Wcast-qual \
 -Wmissing-declarations \
 -Wmissing-prototypes \
 -Wnested-externs \
 -Wpointer-arith \
 -Wstrict-prototypes \
 -Wno-unused-parameter \
 -Wno-unused-function \
 -Wshadow \
 -Wundef \
 -Wwrite-strings

CFLAGS := -g -std=gnu99 $(warn) $(incl) $(opt) $(prof) $(osargs)

.c.o:
	$(CC) $(CFLAGS) $(incl) -c -o ${@F}  $<

all : $(targets)

BEDVCF := bedvcf.o
bedvcf : $(BEDVCF)
	$(CC) $(CFLAGS) -o $@ $(BEDVCF)

clean :
	rm -f *.a *.o *~ gmon.out *.tmp $(targets) core.* vgcore.*

install : $(targets)
	cp $(targets) $(destination)

ginstall : $(targets) $(pytargets)
	cp $(targets) $(global_destination)

.SUFFIXES:
.SUFFIXES: .c .o
.PHONY: clean

